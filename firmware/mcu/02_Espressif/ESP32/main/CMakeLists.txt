idf_component_register(
    SRCS "entry.c" "../board/esp_idf_board.c" "../examples/main_ads1299.c"
         "../ads1299_port/ads1299_spi.c" "../ads1299_port/ads1299_gpio.c"
         "../ads1299_port/ads1299_drdy.c" "../esp_idf_adapter/ads1299_esp32_hal.c"
    INCLUDE_DIRS "../ads1299_port" "../esp_idf_adapter"
    REQUIRES ads1299 driver esp_rom)
set_source_files_properties("../examples/main_ads1299.c" PROPERTIES COMPILE_DEFINITIONS "main=ads1299_example_main")
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
