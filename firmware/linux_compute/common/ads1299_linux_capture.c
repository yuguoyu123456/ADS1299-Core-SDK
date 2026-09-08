#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <linux/gpio.h>
#include <linux/spi/spidev.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define ADS1299_FRAME_BYTES 27u
#define ADS1299_CHANNELS 8u

#define CMD_RESET  0x06u
#define CMD_START  0x08u
#define CMD_STOP   0x0Au
#define CMD_RDATAC 0x10u
#define CMD_SDATAC 0x11u
#define CMD_RREG   0x20u
#define CMD_WREG   0x40u

#define REG_ID        0x00u
#define REG_CONFIG1   0x01u
#define REG_CONFIG2   0x02u
#define REG_CONFIG3   0x03u
#define REG_CH1SET    0x05u
#define REG_BIAS_SENSP 0x0Du
#define REG_BIAS_SENSN 0x0Eu

struct options {
    const char *spi_dev;
    const char *gpio_chip;
    unsigned drdy_line;
    uint32_t spi_hz;
    uint64_t samples;
    const char *profile;
    bool csv;
    bool no_configure;
};

static void die(const char *what) {
    fprintf(stderr, "%s: %s\n", what, strerror(errno));
    exit(EXIT_FAILURE);
}

static void sleep_us(unsigned us) {
    struct timespec ts;
    ts.tv_sec = (time_t)(us / 1000000u);
    ts.tv_nsec = (long)(us % 1000000u) * 1000L;
    while (nanosleep(&ts, &ts) != 0 && errno == EINTR) {}
}

static int spi_transfer(int fd, uint32_t speed_hz,
                        const uint8_t *tx, uint8_t *rx, size_t len) {
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));
    tr.tx_buf = (uintptr_t)tx;
    tr.rx_buf = (uintptr_t)rx;
    tr.len = (uint32_t)len;
    tr.speed_hz = speed_hz;
    tr.bits_per_word = 8;

    int rc = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    return (rc < 0) ? -1 : 0;
}

static int spi_command(int fd, uint32_t speed_hz, uint8_t cmd) {
    uint8_t rx = 0;
    return spi_transfer(fd, speed_hz, &cmd, &rx, 1u);
}

static int write_reg(int fd, uint32_t speed_hz, uint8_t reg, uint8_t value) {
    uint8_t tx[3] = {(uint8_t)(CMD_WREG | (reg & 0x1Fu)), 0x00u, value};
    uint8_t rx[3] = {0};
    return spi_transfer(fd, speed_hz, tx, rx, sizeof(tx));
}

static int read_reg(int fd, uint32_t speed_hz, uint8_t reg, uint8_t *value) {
    uint8_t tx[3] = {(uint8_t)(CMD_RREG | (reg & 0x1Fu)), 0x00u, 0x00u};
    uint8_t rx[3] = {0};
    if (spi_transfer(fd, speed_hz, tx, rx, sizeof(tx)) != 0) return -1;
    *value = rx[2];
    return 0;
}

static int configure_profile(int fd, uint32_t speed_hz, const char *profile) {
    if (spi_command(fd, speed_hz, CMD_SDATAC) != 0) return -1;
    sleep_us(10);

    /* 250 SPS at nominal 2.048 MHz ADS1299 clock. */
    if (write_reg(fd, speed_hz, REG_CONFIG1, 0x96u) != 0) return -1;

    /* Enable internal reference buffer + internal BIAS reference + BIAS buffer.
     * Board designs with an externally driven BIASREF should adjust CONFIG3. */
    if (write_reg(fd, speed_hz, REG_CONFIG3, 0xECu) != 0) return -1;

    uint8_t config2 = 0xC0u;
    uint8_t chset = 0x60u; /* gain 24, normal electrode input */
    uint8_t bias_mask = 0xFFu;

    if (strcmp(profile, "test") == 0) {
        config2 = 0xD0u; /* internal test source, default amplitude/frequency */
        chset = 0x65u;   /* gain 24, test-signal MUX */
        bias_mask = 0x00u;
    } else if (strcmp(profile, "short") == 0) {
        config2 = 0xC0u;
        chset = 0x61u;   /* gain 24, input shorted */
        bias_mask = 0x00u;
    } else if (strcmp(profile, "eeg250") != 0) {
        fprintf(stderr, "unknown profile '%s' (use eeg250, test or short)\n", profile);
        errno = EINVAL;
        return -1;
    }

    if (write_reg(fd, speed_hz, REG_CONFIG2, config2) != 0) return -1;
    for (unsigned ch = 0; ch < ADS1299_CHANNELS; ++ch) {
        if (write_reg(fd, speed_hz, (uint8_t)(REG_CH1SET + ch), chset) != 0) return -1;
    }
    if (write_reg(fd, speed_hz, REG_BIAS_SENSP, bias_mask) != 0) return -1;
    if (write_reg(fd, speed_hz, REG_BIAS_SENSN, bias_mask) != 0) return -1;

    return 0;
}

static int open_spi(const struct options *opt) {
    int fd = open(opt->spi_dev, O_RDWR | O_CLOEXEC);
    if (fd < 0) die("open spidev");

    uint8_t mode = SPI_MODE_1;
    uint8_t bits = 8;
    uint32_t speed = opt->spi_hz;

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) die("SPI_IOC_WR_MODE");
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) die("SPI_IOC_WR_BITS_PER_WORD");
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) die("SPI_IOC_WR_MAX_SPEED_HZ");

    return fd;
}

static int request_drdy_event(const char *chip_path, unsigned line_offset) {
    int chip_fd = open(chip_path, O_RDONLY | O_CLOEXEC);
    if (chip_fd < 0) die("open gpiochip");

    struct gpioevent_request req;
    memset(&req, 0, sizeof(req));
    req.lineoffset = line_offset;
    req.handleflags = GPIOHANDLE_REQUEST_INPUT;
    req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
    snprintf(req.consumer_label, sizeof(req.consumer_label), "ads1299-drdy");

    if (ioctl(chip_fd, GPIO_GET_LINEEVENT_IOCTL, &req) < 0)
        die("GPIO_GET_LINEEVENT_IOCTL");

    close(chip_fd);
    return req.fd;
}

static int wait_drdy(int event_fd) {
    struct pollfd pfd;
    pfd.fd = event_fd;
    pfd.events = POLLIN | POLLPRI;
    pfd.revents = 0;

    for (;;) {
        int rc = poll(&pfd, 1, -1);
        if (rc < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (pfd.revents & (POLLIN | POLLPRI)) {
            struct gpioevent_data event;
            ssize_t n = read(event_fd, &event, sizeof(event));
            if (n == (ssize_t)sizeof(event) && event.id == GPIOEVENT_EVENT_FALLING_EDGE)
                return 0;
        }
    }
}

static int read_frame(int spi_fd, uint32_t speed_hz, uint8_t frame[ADS1299_FRAME_BYTES]) {
    uint8_t zeros[ADS1299_FRAME_BYTES] = {0};
    return spi_transfer(spi_fd, speed_hz, zeros, frame, ADS1299_FRAME_BYTES);
}

static int32_t sign_extend24(const uint8_t *p) {
    uint32_t v = ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | (uint32_t)p[2];
    if (v & 0x00800000u) v |= 0xFF000000u;
    return (int32_t)v;
}

static void emit_csv(uint64_t sample_index, const uint8_t frame[ADS1299_FRAME_BYTES]) {
    printf("%" PRIu64 ",%02X%02X%02X", sample_index, frame[0], frame[1], frame[2]);
    for (unsigned ch = 0; ch < ADS1299_CHANNELS; ++ch) {
        int32_t code = sign_extend24(&frame[3u + ch * 3u]);
        printf(",%" PRId32, code);
    }
    putchar('\n');
}

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s --spi /dev/spidevB.C --gpiochip /dev/gpiochipN --drdy-line N [options]\n"
        "Options:\n"
        "  --speed HZ           SPI clock (default 4000000)\n"
        "  --samples N          number of frames; 0 means continuous (default 0)\n"
        "  --profile NAME       eeg250, test, short (default test)\n"
        "  --no-configure       leave ADS1299 register state unchanged\n"
        "  --csv                print sample,status,ch1..ch8 as CSV\n"
        "  --raw                write exact 27-byte frames to stdout (default)\n"
        "  --help               show this help\n", prog);
}

static struct options parse_args(int argc, char **argv) {
    struct options opt = {
        .spi_dev = NULL,
        .gpio_chip = NULL,
        .drdy_line = 0,
        .spi_hz = 4000000u,
        .samples = 0u,
        .profile = "test",
        .csv = false,
        .no_configure = false,
    };
    bool have_line = false;

    static const struct option long_opts[] = {
        {"spi", required_argument, NULL, 's'},
        {"gpiochip", required_argument, NULL, 'g'},
        {"drdy-line", required_argument, NULL, 'd'},
        {"speed", required_argument, NULL, 'f'},
        {"samples", required_argument, NULL, 'n'},
        {"profile", required_argument, NULL, 'p'},
        {"no-configure", no_argument, NULL, 'N'},
        {"csv", no_argument, NULL, 'c'},
        {"raw", no_argument, NULL, 'r'},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };

    for (;;) {
        int c = getopt_long(argc, argv, "s:g:d:f:n:p:Ncrh", long_opts, NULL);
        if (c == -1) break;
        switch (c) {
            case 's': opt.spi_dev = optarg; break;
            case 'g': opt.gpio_chip = optarg; break;
            case 'd': opt.drdy_line = (unsigned)strtoul(optarg, NULL, 0); have_line = true; break;
            case 'f': opt.spi_hz = (uint32_t)strtoul(optarg, NULL, 0); break;
            case 'n': opt.samples = strtoull(optarg, NULL, 0); break;
            case 'p': opt.profile = optarg; break;
            case 'N': opt.no_configure = true; break;
            case 'c': opt.csv = true; break;
            case 'r': opt.csv = false; break;
            case 'h': usage(argv[0]); exit(EXIT_SUCCESS);
            default: usage(argv[0]); exit(EXIT_FAILURE);
        }
    }

    if (!opt.spi_dev || !opt.gpio_chip || !have_line || opt.spi_hz == 0u) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    return opt;
}

int main(int argc, char **argv) {
    struct options opt = parse_args(argc, argv);
    int spi_fd = open_spi(&opt);
    int drdy_fd = request_drdy_event(opt.gpio_chip, opt.drdy_line);

    if (!opt.no_configure) {
        if (spi_command(spi_fd, opt.spi_hz, CMD_RESET) != 0) die("ADS1299 RESET");
        sleep_us(1000);

        if (spi_command(spi_fd, opt.spi_hz, CMD_SDATAC) != 0) die("ADS1299 SDATAC");
        sleep_us(10);

        uint8_t id = 0;
        if (read_reg(spi_fd, opt.spi_hz, REG_ID, &id) != 0) die("ADS1299 read ID");
        fprintf(stderr, "ADS1299 ID register: 0x%02X\n", id);

        if (configure_profile(spi_fd, opt.spi_hz, opt.profile) != 0)
            die("ADS1299 configure profile");
    } else {
        if (spi_command(spi_fd, opt.spi_hz, CMD_SDATAC) != 0) die("ADS1299 SDATAC");
    }

    if (spi_command(spi_fd, opt.spi_hz, CMD_RDATAC) != 0) die("ADS1299 RDATAC");
    sleep_us(10);
    if (spi_command(spi_fd, opt.spi_hz, CMD_START) != 0) die("ADS1299 START");

    if (opt.csv)
        puts("sample,status,ch1,ch2,ch3,ch4,ch5,ch6,ch7,ch8");

    uint64_t index = 0;
    while (opt.samples == 0u || index < opt.samples) {
        uint8_t frame[ADS1299_FRAME_BYTES];
        if (wait_drdy(drdy_fd) != 0) die("wait DRDY");
        if (read_frame(spi_fd, opt.spi_hz, frame) != 0) die("read ADS1299 frame");

        if (opt.csv) {
            emit_csv(index, frame);
        } else {
            if (fwrite(frame, 1, sizeof(frame), stdout) != sizeof(frame))
                die("write stdout");
        }
        ++index;
    }

    (void)spi_command(spi_fd, opt.spi_hz, CMD_STOP);
    (void)spi_command(spi_fd, opt.spi_hz, CMD_SDATAC);
    close(drdy_fd);
    close(spi_fd);
    return 0;
}
