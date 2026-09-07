idf_component_register(
    SRCS "ads1299_esp_idf_hal.c"
    INCLUDE_DIRS "."
    REQUIRES driver ads1299_port
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
