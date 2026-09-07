cmake_minimum_required(VERSION 3.16)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
project(ads1299_rt1170_official_sdk_check C ASM)
foreach(required MCUX_SDK_ROOT CMSIS_CORE_INCLUDE)
    if(NOT DEFINED ${required})
        message(FATAL_ERROR "Set ${required} to the installed official dependency")
    endif()
endforeach()
add_library(ads1299_official_sdk INTERFACE)
target_include_directories(ads1299_official_sdk SYSTEM INTERFACE
    ${MCUX_SDK_ROOT}/devices/MIMXRT1176
    ${MCUX_SDK_ROOT}/devices/MIMXRT1176/drivers
    ${MCUX_SDK_ROOT}/drivers/common
    ${MCUX_SDK_ROOT}/drivers/igpio
    ${MCUX_SDK_ROOT}/drivers/lpspi
    ${CMSIS_CORE_INCLUDE})
target_compile_definitions(ads1299_official_sdk INTERFACE CPU_MIMXRT1176DVMAA_cm7)
target_compile_options(ads1299_official_sdk INTERFACE
    -mcpu=cortex-m7 -mthumb -Wall -Wextra -Werror)
add_subdirectory(../.. rt1170)
# Object compilation only: no fabricated board binding to hide missing symbols.
add_library(ads1299_example_check OBJECT ../../examples/main_ads1299.c)
target_link_libraries(ads1299_example_check PRIVATE ads1299_rt1170)

option(ADS1299_LINK_REFERENCE_RAM "Link an EVK-derived RAM-debug reference image" OFF)
if(ADS1299_LINK_REFERENCE_RAM)
    # All vendor sources, startup and linker files remain in the external SDK.
    add_executable(ads1299_evk_reference.elf
        ../../board/evk_reference.c ../../examples/main_ads1299.c
        ${MCUX_SDK_ROOT}/devices/MIMXRT1176/system_MIMXRT1176_cm7.c
        ${MCUX_SDK_ROOT}/devices/MIMXRT1176/gcc/startup_MIMXRT1176_cm7.S
        ${MCUX_SDK_ROOT}/devices/MIMXRT1176/drivers/fsl_clock.c
        ${MCUX_SDK_ROOT}/devices/MIMXRT1176/drivers/fsl_pmu.c
        ${MCUX_SDK_ROOT}/devices/MIMXRT1176/drivers/fsl_anatop_ai.c
        ${MCUX_SDK_ROOT}/drivers/common/fsl_common.c
        ${MCUX_SDK_ROOT}/drivers/common/fsl_common_arm.c
        ${MCUX_SDK_ROOT}/drivers/igpio/fsl_gpio.c
        ${MCUX_SDK_ROOT}/drivers/lpspi/fsl_lpspi.c)
    target_link_libraries(ads1299_evk_reference.elf PRIVATE ads1299_rt1170)
    # SDK 2.16 has intentionally unused parameters. Keep warnings visible,
    # relax only that category in external SDK sources, never repository code.
    set_source_files_properties(
        ${MCUX_SDK_ROOT}/devices/MIMXRT1176/drivers/fsl_clock.c
        ${MCUX_SDK_ROOT}/devices/MIMXRT1176/drivers/fsl_pmu.c
        ${MCUX_SDK_ROOT}/drivers/lpspi/fsl_lpspi.c
        PROPERTIES COMPILE_OPTIONS "-Wno-error=unused-parameter")
    target_compile_definitions(ads1299_evk_reference.elf PRIVATE SPI_RETRY_TIMES=1000000)
    target_compile_options(ads1299_evk_reference.elf PRIVATE -ffunction-sections -fdata-sections)
    target_link_options(ads1299_evk_reference.elf PRIVATE
        -mcpu=cortex-m7 -mthumb --specs=nano.specs --specs=nosys.specs
        -Wl,--gc-sections
        -T${MCUX_SDK_ROOT}/devices/MIMXRT1176/gcc/MIMXRT1176xxxxx_cm7_ram.ld)
endif()
