add_library(ads1299_control STATIC
    ads1299_control.c
    ads1299_cli.c
)

target_include_directories(ads1299_control PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../core_driver/ads1299
)
