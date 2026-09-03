cmake_minimum_required(VERSION 3.13)

if (NOT PICO_SDK_PATH)
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()
if (NOT PICO_SDK_PATH)
    message(FATAL_ERROR "PICO_SDK_PATH is not set")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ads1299_core_rp2040 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(ads1299_core_rp2040
    src/main.c
    ../../drivers/ads1299/ads1299.c
    ../../drivers/ads1299/ads1299_bringup.c
    ../../drivers/ads1299/ads1299_control.c
    ../../ports/rp2xxx/ads1299_port_pico.c
    ../../host/c/ads1299_packet.c
)

target_include_directories(ads1299_core_rp2040 PRIVATE
    ../../drivers/ads1299
    ../../ports/rp2xxx
    ../../host/c
)

target_link_libraries(ads1299_core_rp2040 PRIVATE
    pico_stdlib
    hardware_spi
    hardware_sync
    hardware_uart
)

# USB CDC is used for human-readable diagnostics; UART0 carries binary packets.
pico_enable_stdio_usb(ads1299_core_rp2040 1)
pico_enable_stdio_uart(ads1299_core_rp2040 0)
pico_add_extra_outputs(ads1299_core_rp2040)
