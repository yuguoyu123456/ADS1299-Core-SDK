#ifndef ADS1299_RA6M5_BOARD_SDK_H
#define ADS1299_RA6M5_BOARD_SDK_H
#include <stdint.h>
typedef enum { BOARD_PIN_SCK, BOARD_PIN_MISO, BOARD_PIN_MOSI, BOARD_PIN_CS, BOARD_PIN_DRDY, BOARD_PIN_RESET, BOARD_PIN_PWDN, BOARD_PIN_START, BOARD_PIN_TX } board_sdk_pin_t;
#define BOARD_SDK_NAME "Renesas EK-RA6M5"
int board_sdk_init(void); void board_sdk_write_pin(board_sdk_pin_t pin,int level); int board_sdk_read_pin(board_sdk_pin_t pin); void board_sdk_delay_us(uint32_t us); void board_sdk_fatal(const char *reason);
#endif
