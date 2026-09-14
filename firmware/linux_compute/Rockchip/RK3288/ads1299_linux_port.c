#define _POSIX_C_SOURCE 200809L

#include "ads1299_linux_port.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/gpio.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

static void init_fds(rk3288_ads1299_linux_t *ctx) {
    ctx->spi_fd = -1;
    ctx->cs_fd = -1;
    ctx->reset_fd = -1;
    ctx->pwdn_fd = -1;
    ctx->start_fd = -1;
    ctx->drdy_fd = -1;
}

static int request_line(int chip_fd, unsigned int offset, unsigned long flags,
                        int default_value, const char *label) {
    struct gpiohandle_request request;
    memset(&request, 0, sizeof(request));
    request.lineoffsets[0] = offset;
    request.flags = flags;
    request.lines = 1;
    request.default_values[0] = default_value ? 1u : 0u;
    strncpy(request.consumer_label, label,
            sizeof(request.consumer_label) - 1u);

    if (ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &request) < 0) {
        return -errno;
    }
    return request.fd;
}

static int request_optional_line(int chip_fd, unsigned int offset,
                                 unsigned long flags, int default_value,
                                 const char *label) {
    if (offset == UINT_MAX) return -1;
    return request_line(chip_fd, offset, flags, default_value, label);
}

static int set_line(int fd, int level) {
    if (fd < 0) return -ENODEV;
    struct gpiohandle_data data;
    memset(&data, 0, sizeof(data));
    data.values[0] = level ? 1u : 0u;
    if (ioctl(fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
        return -errno;
    }
    return 0;
}

static int get_line(int fd) {
    if (fd < 0) return -ENODEV;
    struct gpiohandle_data data;
    memset(&data, 0, sizeof(data));
    if (ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
        return -errno;
    }
    return data.values[0] ? 1 : 0;
}

static int spi_transfer_cb(void *user, const uint8_t *tx, uint8_t *rx,
                           size_t len) {
    rk3288_ads1299_linux_t *ctx = (rk3288_ads1299_linux_t *)user;
    if (!ctx || ctx->spi_fd < 0 || len == 0u) return -EINVAL;

    struct spi_ioc_transfer transfer;
    memset(&transfer, 0, sizeof(transfer));
    transfer.tx_buf = (uintptr_t)tx;
    transfer.rx_buf = (uintptr_t)rx;
    transfer.len = (uint32_t)len;
    transfer.speed_hz = ctx->spi_hz;
    transfer.bits_per_word = 8u;

    const int rc = ioctl(ctx->spi_fd, SPI_IOC_MESSAGE(1), &transfer);
    return rc < 0 ? -errno : 0;
}

static void cs_write_cb(void *user, int level) {
    rk3288_ads1299_linux_t *ctx = (rk3288_ads1299_linux_t *)user;
    if (ctx) (void)set_line(ctx->cs_fd, level);
}

static void reset_write_cb(void *user, int level) {
    rk3288_ads1299_linux_t *ctx = (rk3288_ads1299_linux_t *)user;
    if (ctx && ctx->reset_fd >= 0) (void)set_line(ctx->reset_fd, level);
}

static void pwdn_write_cb(void *user, int level) {
    rk3288_ads1299_linux_t *ctx = (rk3288_ads1299_linux_t *)user;
    if (ctx && ctx->pwdn_fd >= 0) (void)set_line(ctx->pwdn_fd, level);
}

static void start_write_cb(void *user, int level) {
    rk3288_ads1299_linux_t *ctx = (rk3288_ads1299_linux_t *)user;
    if (ctx && ctx->start_fd >= 0) (void)set_line(ctx->start_fd, level);
}

static int drdy_read_cb(void *user) {
    rk3288_ads1299_linux_t *ctx = (rk3288_ads1299_linux_t *)user;
    return ctx ? get_line(ctx->drdy_fd) : -EINVAL;
}

static void delay_us_cb(void *user, uint32_t us) {
    (void)user;
    struct timespec req;
    req.tv_sec = (time_t)(us / 1000000u);
    req.tv_nsec = (long)((us % 1000000u) * 1000u);
    while (nanosleep(&req, &req) < 0 && errno == EINTR) {
    }
}

int rk3288_ads1299_linux_open(rk3288_ads1299_linux_t *ctx,
                              const rk3288_ads1299_linux_config_t *config) {
    if (!ctx || !config || !config->spidev_path || !config->gpiochip_path ||
        config->spi_hz == 0u || config->cs_line == UINT_MAX) {
        return -EINVAL;
    }

    init_fds(ctx);
    ctx->spi_hz = config->spi_hz;

    ctx->spi_fd = open(config->spidev_path, O_RDWR | O_CLOEXEC);
    if (ctx->spi_fd < 0) return -errno;

    uint8_t mode = (uint8_t)(SPI_MODE_1 | SPI_NO_CS);
    uint8_t bits = 8u;
    uint32_t speed = config->spi_hz;
    if (ioctl(ctx->spi_fd, SPI_IOC_WR_MODE, &mode) < 0 ||
        ioctl(ctx->spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 ||
        ioctl(ctx->spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        const int saved = errno;
        rk3288_ads1299_linux_close(ctx);
        return -saved;
    }

    const int chip_fd = open(config->gpiochip_path, O_RDONLY | O_CLOEXEC);
    if (chip_fd < 0) {
        const int saved = errno;
        rk3288_ads1299_linux_close(ctx);
        return -saved;
    }

    ctx->cs_fd = request_line(chip_fd, config->cs_line,
                              GPIOHANDLE_REQUEST_OUTPUT, 1, "ads1299-cs");
    if (ctx->cs_fd < 0) {
        const int saved = -ctx->cs_fd;
        close(chip_fd);
        rk3288_ads1299_linux_close(ctx);
        return -saved;
    }

    ctx->reset_fd = request_optional_line(chip_fd, config->reset_line,
                                          GPIOHANDLE_REQUEST_OUTPUT, 1,
                                          "ads1299-reset");
    ctx->pwdn_fd = request_optional_line(chip_fd, config->pwdn_line,
                                         GPIOHANDLE_REQUEST_OUTPUT, 1,
                                         "ads1299-pwdn");
    ctx->start_fd = request_optional_line(chip_fd, config->start_line,
                                          GPIOHANDLE_REQUEST_OUTPUT, 0,
                                          "ads1299-start");
    ctx->drdy_fd = request_optional_line(chip_fd, config->drdy_line,
                                         GPIOHANDLE_REQUEST_INPUT, 0,
                                         "ads1299-drdy");
    close(chip_fd);

    if ((config->reset_line != UINT_MAX && ctx->reset_fd < 0) ||
        (config->pwdn_line != UINT_MAX && ctx->pwdn_fd < 0) ||
        (config->start_line != UINT_MAX && ctx->start_fd < 0) ||
        (config->drdy_line != UINT_MAX && ctx->drdy_fd < 0)) {
        rk3288_ads1299_linux_close(ctx);
        return -EIO;
    }

    return 0;
}

void rk3288_ads1299_linux_close(rk3288_ads1299_linux_t *ctx) {
    if (!ctx) return;
    if (ctx->drdy_fd >= 0) close(ctx->drdy_fd);
    if (ctx->start_fd >= 0) close(ctx->start_fd);
    if (ctx->pwdn_fd >= 0) close(ctx->pwdn_fd);
    if (ctx->reset_fd >= 0) close(ctx->reset_fd);
    if (ctx->cs_fd >= 0) close(ctx->cs_fd);
    if (ctx->spi_fd >= 0) close(ctx->spi_fd);
    init_fds(ctx);
    ctx->spi_hz = 0u;
}

int rk3288_ads1299_linux_bind(rk3288_ads1299_linux_t *ctx,
                              ads1299_port_t *port) {
    if (!ctx || !port || ctx->spi_fd < 0 || ctx->cs_fd < 0) return -EINVAL;

    memset(port, 0, sizeof(*port));
    port->user = ctx;
    port->spi_transfer = spi_transfer_cb;
    port->cs_write = cs_write_cb;
    port->reset_write = ctx->reset_fd >= 0 ? reset_write_cb : NULL;
    port->pwdn_write = ctx->pwdn_fd >= 0 ? pwdn_write_cb : NULL;
    port->start_write = ctx->start_fd >= 0 ? start_write_cb : NULL;
    port->drdy_read = ctx->drdy_fd >= 0 ? drdy_read_cb : NULL;
    port->delay_us = delay_us_cb;
    return 0;
}
