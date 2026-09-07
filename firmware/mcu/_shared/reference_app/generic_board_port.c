#include "generic_board_port.h"

/* Every source-complete/build-pending project contributes a concrete
 * port/board_sdk.h on its include path. That header owns GPIO setup and
 * microsecond delay for the exact MCU family. */
#include "board_sdk.h"

#ifndef BOARD_SOFT_SPI_HALF_US
#define BOARD_SOFT_SPI_HALF_US 2u
#endif
#ifndef BOARD_SOFT_UART_BIT_US
#define BOARD_SOFT_UART_BIT_US 2u /* 500000 baud, 8-N-1. */
#endif

static int soft_spi_transfer(void *user,
                             const uint8_t *tx,
                             uint8_t *rx,
                             size_t len) {
    (void)user;
    for (size_t i = 0; i < len; ++i) {
        const uint8_t out = tx ? tx[i] : 0u;
        uint8_t in = 0u;
        for (uint8_t mask = 0x80u; mask != 0u; mask >>= 1u) {
            /* ADS1299 SPI mode 1: CPOL=0, DIN is captured on the falling edge
             * and DOUT changes after the rising edge. Keep MOSI stable during
             * the high phase and sample MISO immediately before the falling
             * edge. */
            board_sdk_write_pin(BOARD_PIN_MOSI, (out & mask) ? 1 : 0);
            board_sdk_write_pin(BOARD_PIN_SCK, 1);
            board_sdk_delay_us(BOARD_SOFT_SPI_HALF_US);
            if (board_sdk_read_pin(BOARD_PIN_MISO)) in |= mask;
            board_sdk_write_pin(BOARD_PIN_SCK, 0);
            board_sdk_delay_us(BOARD_SOFT_SPI_HALF_US);
        }
        if (rx) rx[i] = in;
    }
    return 0;
}

static void pin_write(void *user, board_sdk_pin_t pin, int level) {
    (void)user;
    board_sdk_write_pin(pin, level ? 1 : 0);
}
static void cs_write(void *user, int level) { pin_write(user, BOARD_PIN_CS, level); }
static void reset_write(void *user, int level) { pin_write(user, BOARD_PIN_RESET, level); }
static void pwdn_write(void *user, int level) { pin_write(user, BOARD_PIN_PWDN, level); }
static void start_write(void *user, int level) { pin_write(user, BOARD_PIN_START, level); }
static int drdy_read(void *user) {
    (void)user;
    return board_sdk_read_pin(BOARD_PIN_DRDY) ? 1 : 0;
}
static void delay_us(void *user, uint32_t us) {
    (void)user;
    board_sdk_delay_us(us);
}

static ads1299_port_t make_port(void *user) {
    ads1299_port_t port = {
        .user = user,
        .spi_transfer = soft_spi_transfer,
        .cs_write = cs_write,
        .reset_write = reset_write,
        .pwdn_write = pwdn_write,
        .start_write = start_write,
        .drdy_read = drdy_read,
        .delay_us = delay_us,
    };
    return port;
}

static int board_init(void *user) {
    (void)user;
    if (board_sdk_init() != 0) return -1;
    board_sdk_write_pin(BOARD_PIN_SCK, 0);
    board_sdk_write_pin(BOARD_PIN_MOSI, 0);
    board_sdk_write_pin(BOARD_PIN_TX, 1);
    board_sdk_write_pin(BOARD_PIN_CS, 1);
    board_sdk_write_pin(BOARD_PIN_RESET, 1);
    board_sdk_write_pin(BOARD_PIN_PWDN, 1);
    board_sdk_write_pin(BOARD_PIN_START, 0);
    return 0;
}

static void soft_uart_write_byte(uint8_t value) {
    board_sdk_write_pin(BOARD_PIN_TX, 0); /* start */
    board_sdk_delay_us(BOARD_SOFT_UART_BIT_US);
    for (unsigned bit = 0; bit < 8u; ++bit) {
        board_sdk_write_pin(BOARD_PIN_TX, (value >> bit) & 1u);
        board_sdk_delay_us(BOARD_SOFT_UART_BIT_US);
    }
    board_sdk_write_pin(BOARD_PIN_TX, 1); /* stop */
    board_sdk_delay_us(BOARD_SOFT_UART_BIT_US);
}

static int stream_write(void *user, const uint8_t *data, size_t len) {
    (void)user;
    if (!data) return -1;
    for (size_t i = 0; i < len; ++i) soft_uart_write_byte(data[i]);
    return 0;
}

static void fatal(void *user, const char *reason) {
    (void)user;
    board_sdk_fatal(reason);
}

const ads1299_reference_board_t *ads1299_board_instance(void) {
    static const ads1299_reference_board_t board = {
        .name = BOARD_SDK_NAME,
        .user = NULL,
        .init = board_init,
        .make_port = make_port,
        .stream_write = stream_write,
        .fatal = fatal,
    };
    return &board;
}
