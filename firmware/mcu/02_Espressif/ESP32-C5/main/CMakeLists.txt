idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES ads1299 ads1299_port
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Werror)
