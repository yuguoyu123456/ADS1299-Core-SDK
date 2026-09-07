cmake_minimum_required(VERSION 3.16)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
project(ads1299_mspm0_reference C)
foreach(required TI_SDK_ROOT CMSIS_CORE_INCLUDE)
    if(NOT DEFINED ${required})
        message(FATAL_ERROR "Set ${required} to the installed official dependency")
    endif()
endforeach()
add_library(ads1299_official_sdk INTERFACE)
target_include_directories(ads1299_official_sdk SYSTEM INTERFACE
    ${TI_SDK_ROOT}/source ${CMSIS_CORE_INCLUDE})
target_compile_definitions(ads1299_official_sdk INTERFACE __MSPM0G3507__)
target_compile_options(ads1299_official_sdk INTERFACE
    -mcpu=cortex-m0plus -mthumb -Wall -Wextra -Werror -ffunction-sections -fdata-sections)
add_subdirectory(../.. mspm0)
add_executable(ads1299_launchpad_reference.elf
    ../../board/launchpad_reference.c ../../examples/main_ads1299.c
    ${TI_SDK_ROOT}/source/ti/driverlib/dl_spi.c
    ${TI_SDK_ROOT}/source/ti/driverlib/dl_gpio.c
    ${TI_SDK_ROOT}/source/ti/driverlib/dl_common.c
    ${TI_SDK_ROOT}/source/ti/driverlib/m0p/sysctl/dl_sysctl_mspm0g1x0x_g3x0x.c
    ${TI_SDK_ROOT}/source/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0g350x_gcc.c)
target_link_libraries(ads1299_launchpad_reference.elf PRIVATE ads1299_mspm0)
target_link_options(ads1299_launchpad_reference.elf PRIVATE
    -mcpu=cortex-m0plus -mthumb --specs=nano.specs --specs=nosys.specs
    -Wl,--gc-sections -Wl,-e,Reset_Handler
    -T${TI_SDK_ROOT}/source/ti/devices/msp/m0p/linker_files/gcc/mspm0g3507.lds)
