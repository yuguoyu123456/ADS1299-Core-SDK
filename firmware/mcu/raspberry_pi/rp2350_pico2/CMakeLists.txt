cmake_minimum_required(VERSION 3.13)

if (NOT PICO_SDK_PATH)
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()
if (NOT PICO_SDK_PATH)
    message(FATAL_ERROR "PICO_SDK_PATH is not set")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ads1299_core_rp2350 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(ads1299_core_rp2350
    src/main.c
    ../../../core_driver/ads1299/ads1299.c
    ../../../core_driver/ads1299/ads1299_bringup.c
    ../../../core_driver/ads1299/ads1299_control.c
    ../../../core_driver/ads1299/ads1299_field_model.c
    ../../../core_driver/ads1299/ads1299_register_model.c
    ../../../core_driver/ads1299/ads1299_runtime.c
    port/ads1299_port_pico.c
    ../../../common/data_packet/ads1299_packet.c
)

target_include_directories(ads1299_core_rp2350 PRIVATE
    ../../../core_driver/ads1299
    port
    ../../../common/data_packet
)

target_link_libraries(ads1299_core_rp2350 PRIVATE pico_stdlib hardware_spi hardware_sync hardware_uart)
pico_enable_stdio_usb(ads1299_core_rp2350 1)
pico_enable_stdio_uart(ads1299_core_rp2350 0)
pico_add_extra_outputs(ads1299_core_rp2350)
