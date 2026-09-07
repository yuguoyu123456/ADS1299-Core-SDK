idf_component_register(
    SRCS "ads1299_spi.c" "ads1299_gpio.c" "ads1299_drdy.c"
    INCLUDE_DIRS "."
    REQUIRES ads1299
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
