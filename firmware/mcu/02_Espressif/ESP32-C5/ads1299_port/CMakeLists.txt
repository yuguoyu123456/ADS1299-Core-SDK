idf_component_register(
    SRCS
        "ads1299_spi.c"
        "ads1299_gpio.c"
        "ads1299_drdy.c"
        "ads1299_espidf_hal.c"
    INCLUDE_DIRS "." "../board"
    REQUIRES ads1299 esp_driver_spi esp_driver_gpio esp_rom
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
