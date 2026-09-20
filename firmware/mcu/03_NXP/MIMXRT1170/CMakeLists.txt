# Include from an existing official SDK Cortex-M7 application. This is a
# library integration target, not a standalone board/startup/linker project.
# The parent SDK application must supply its SDK include/compile usage target.
# Never silently fall back to mock headers or a host compiler.
if(NOT TARGET ads1299_official_sdk)
    message(FATAL_ERROR "Define an ads1299_official_sdk INTERFACE target with official SDK includes, CPU and compiler options before add_subdirectory")
endif()

add_library(ads1299_rt1170 STATIC
    ../../../core_driver/ads1299/ads1299.c
    ../../../core_driver/ads1299/ads1299_model.c
    ../../../core_driver/ads1299/ads1299_frame.c
    ../../../core_driver/ads1299/ads1299_multi.c
    ads1299_port/ads1299_spi.c
    ads1299_port/ads1299_gpio.c
    ads1299_port/ads1299_drdy.c
    mcuxpresso_adapter/ads1299_mcuxpresso_hal.c
    mcuxpresso_adapter/ads1299_mcuxpresso_init.c)
target_compile_features(ads1299_rt1170 PUBLIC c_std_11)
target_include_directories(ads1299_rt1170 PUBLIC
    ../../../core_driver/ads1299 ads1299_port mcuxpresso_adapter)
target_link_libraries(ads1299_rt1170 PUBLIC ads1299_official_sdk)

# The existing EVK reference binding is deliberately opt-in: its source file
# documents that the signal mapping is a reference mapping, not verified EVKB
# wiring. Parent projects may instead provide their own board_ads1299_hal().
option(ADS1299_RT1170_ENABLE_EVK_REFERENCE "Build the documented MIMXRT1170 EVK reference ADS1299 board binding" OFF)
if(ADS1299_RT1170_ENABLE_EVK_REFERENCE)
    target_sources(ads1299_rt1170 PRIVATE board/evk_reference.c)
    target_include_directories(ads1299_rt1170 PUBLIC board)
endif()

# Compile every progressive/runnable example against the same shared core and
# official-SDK contract. Each example is an independent OBJECT target because
# it intentionally owns main(). This catches API/include regressions without
# pretending to provide vendor startup/linker files or hardware validation.
option(ADS1299_RT1170_EXAMPLE_SMOKE "Compile all MIMXRT1170 ADS1299 examples" OFF)
if(ADS1299_RT1170_EXAMPLE_SMOKE)
    if(NOT ADS1299_RT1170_ENABLE_EVK_REFERENCE)
        message(FATAL_ERROR "ADS1299_RT1170_EXAMPLE_SMOKE requires ADS1299_RT1170_ENABLE_EVK_REFERENCE=ON, or integrate equivalent example targets in the parent project with its own board_ads1299_hal()")
    endif()

    set(ADS1299_RT1170_EXAMPLE_SOURCES
        examples/main_ads1299.c
        examples/probe_id.c
        examples/internal_test.c
        examples/input_short.c
        examples/eeg_250sps.c
        examples/stream_bounded.c)

    set(ADS1299_RT1170_EXAMPLE_TARGETS "")
    foreach(example_source IN LISTS ADS1299_RT1170_EXAMPLE_SOURCES)
        get_filename_component(example_name "${example_source}" NAME_WE)
        set(example_target "ads1299_rt1170_example_${example_name}")
        add_library(${example_target} OBJECT "${example_source}")
        target_compile_features(${example_target} PRIVATE c_std_11)
        target_include_directories(${example_target} PRIVATE
            ../../../core_driver/ads1299
            ads1299_port
            mcuxpresso_adapter
            board
            examples)
        target_link_libraries(${example_target} PRIVATE
            ads1299_rt1170
            ads1299_official_sdk)
        list(APPEND ADS1299_RT1170_EXAMPLE_TARGETS ${example_target})
    endforeach()

    add_custom_target(ads1299_rt1170_examples_smoke ALL
        DEPENDS ${ADS1299_RT1170_EXAMPLE_TARGETS})
endif()
