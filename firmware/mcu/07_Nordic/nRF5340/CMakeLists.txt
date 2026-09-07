cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ads1299_nrf5340_dk)

set(ADS1299_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../core_driver/ads1299)

target_include_directories(app PRIVATE
    ${ADS1299_CORE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ads1299_port
)

target_sources(app PRIVATE
    zephyr_adapter/ads1299_zephyr_hal.c
    examples/main_ads1299.c
    ads1299_port/ads1299_spi.c
    ads1299_port/ads1299_gpio.c
    ads1299_port/ads1299_drdy.c
    ${ADS1299_CORE_DIR}/ads1299.c
    ${ADS1299_CORE_DIR}/ads1299_frame.c
    ${ADS1299_CORE_DIR}/ads1299_model.c
    ${ADS1299_CORE_DIR}/ads1299_multi.c
)
