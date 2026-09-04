cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ads1299_nrf52840_dk)

set(ADS1299_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../core_driver/ads1299)
set(ADS1299_PACKET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/data_packet)
set(ADS1299_REF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../common/reference_app)

target_include_directories(app PRIVATE
    ${ADS1299_CORE_DIR}
    ${ADS1299_PACKET_DIR}
    ${ADS1299_REF_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/port
)

target_sources(app PRIVATE
    src/board_sdk.c
    ${ADS1299_REF_DIR}/generic_main.c
    ${ADS1299_REF_DIR}/generic_board_port.c
    ${ADS1299_REF_DIR}/ads1299_reference_app.c
    ${ADS1299_PACKET_DIR}/ads1299_packet.c
    ${ADS1299_CORE_DIR}/ads1299.c
    ${ADS1299_CORE_DIR}/ads1299_bringup.c
    ${ADS1299_CORE_DIR}/ads1299_control.c
    ${ADS1299_CORE_DIR}/ads1299_conversion.c
    ${ADS1299_CORE_DIR}/ads1299_diagnostics.c
    ${ADS1299_CORE_DIR}/ads1299_field_model.c
    ${ADS1299_CORE_DIR}/ads1299_frame.c
    ${ADS1299_CORE_DIR}/ads1299_register_model.c
    ${ADS1299_CORE_DIR}/ads1299_runtime.c
    ${ADS1299_CORE_DIR}/ads1299_semantics.c
)
