# MIMXRT1166 ADS1299 library integration for an existing official MCUXpresso
# Cortex-M7 application. Vendor startup, linker, clock and generated board
# sources remain owned by the parent SDK project.

if(NOT TARGET ads1299_official_sdk)
    message(FATAL_ERROR "Define an ads1299_official_sdk INTERFACE target with official MIMXRT1166 SDK includes, CPU and compiler options before add_subdirectory")
endif()

add_library(ads1299_rt1166 STATIC
    ../../../core_driver/ads1299/ads1299.c
    ../../../core_driver/ads1299/ads1299_model.c
    ../../../core_driver/ads1299/ads1299_frame.c
    ../../../core_driver/ads1299/ads1299_multi.c
    ads1299_port/ads1299_spi.c
    ads1299_port/ads1299_gpio.c
    ads1299_port/ads1299_drdy.c
    mcuxpresso_adapter/ads1299_mcuxpresso_hal.c)

target_compile_features(ads1299_rt1166 PUBLIC c_std_11)
target_include_directories(ads1299_rt1166 PUBLIC
    ../../../core_driver/ads1299
    ads1299_port
    mcuxpresso_adapter)
target_link_libraries(ads1299_rt1166 PUBLIC ads1299_official_sdk)

# Board binding is opt-in because this leaf intentionally does not invent a
# fixed EVK pinout. The parent official SDK project must define the hardware
# macros required by board/board_ads1299_config.h.
option(ADS1299_RT1166_ENABLE_BOARD_BINDING "Build the MIMXRT1166 ADS1299 board/config binding" OFF)
if(ADS1299_RT1166_ENABLE_BOARD_BINDING)
    target_sources(ads1299_rt1166 PRIVATE board/board_ads1299_binding.c)
    target_include_directories(ads1299_rt1166 PUBLIC board)
endif()

# Compile the existing runnable integration example against exactly the same
# shared core and official-SDK usage requirements. This is a compile contract,
# not a BOARD-VERIFIED claim and it does not supply vendor startup/linker code.
option(ADS1299_RT1166_EXAMPLE_SMOKE "Compile the MIMXRT1166 ADS1299 integration example" OFF)
if(ADS1299_RT1166_EXAMPLE_SMOKE)
    if(NOT ADS1299_RT1166_ENABLE_BOARD_BINDING)
        message(FATAL_ERROR "ADS1299_RT1166_EXAMPLE_SMOKE requires ADS1299_RT1166_ENABLE_BOARD_BINDING=ON")
    endif()
    add_library(ads1299_rt1166_example_smoke OBJECT
        examples/main_ads1299.c)
    target_compile_features(ads1299_rt1166_example_smoke PRIVATE c_std_11)
    target_include_directories(ads1299_rt1166_example_smoke PRIVATE
        ../../../core_driver/ads1299
        ads1299_port
        mcuxpresso_adapter
        board
        examples)
    target_link_libraries(ads1299_rt1166_example_smoke PRIVATE
        ads1299_rt1166
        ads1299_official_sdk)
endif()
