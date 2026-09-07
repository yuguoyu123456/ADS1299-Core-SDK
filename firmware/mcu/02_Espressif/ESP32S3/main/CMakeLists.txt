idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES driver ads1299 ads1299_port ads1299_esp_idf_hal
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
