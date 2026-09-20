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

# Compile-only integration contract. This is deliberately separate from the
# runnable examples: it gives parent MCUXpresso projects a small target that
# verifies the board macro contract and adapter headers without claiming a
# flashable image or hardware validation.
option(ADS1299_RT1166_COMPILE_SMOKE "Compile MIMXRT1166 ADS1299 integration contract checks" OFF)
if(ADS1299_RT1166_COMPILE_SMOKE)
    if(NOT ADS1299_RT1166_ENABLE_BOARD_BINDING)
        message(FATAL_ERROR "ADS1299_RT1166_COMPILE_SMOKE requires ADS1299_RT1166_ENABLE_BOARD_BINDING=ON")
    endif()
    add_library(ads1299_rt1166_compile_smoke OBJECT
        board/board_ads1299_config_check.c)
    target_compile_features(ads1299_rt1166_compile_smoke PRIVATE c_std_11)
    target_include_directories(ads1299_rt1166_compile_smoke PRIVATE
        ../../../core_driver/ads1299
        ads1299_port
        mcuxpresso_adapter
        board)
    target_link_libraries(ads1299_rt1166_compile_smoke PRIVATE
        ads1299_rt1166
        ads1299_official_sdk)
endif()

# Compile every runnable/progressive example against exactly the same shared
# core, board binding and official-SDK contract. Each source is an independent
# OBJECT target because examples intentionally define their own main(). This
# catches API/include regressions without pretending to provide vendor startup,
# linker scripts, a flashable image, or BOARD-VERIFIED hardware evidence.
option(ADS1299_RT1166_EXAMPLE_SMOKE "Compile all MIMXRT1166 ADS1299 examples" OFF)
if(ADS1299_RT1166_EXAMPLE_SMOKE)
    if(NOT ADS1299_RT1166_ENABLE_BOARD_BINDING)
        message(FATAL_ERROR "ADS1299_RT1166_EXAMPLE_SMOKE requires ADS1299_RT1166_ENABLE_BOARD_BINDING=ON")
    endif()

    set(ADS1299_RT1166_EXAMPLE_SOURCES
        examples/main_ads1299.c
        examples/probe_id.c
        examples/internal_test.c
        examples/input_short.c
        examples/eeg_250sps.c
        examples/stream_bounded.c)

    set(ADS1299_RT1166_EXAMPLE_TARGETS "")
    foreach(example_source IN LISTS ADS1299_RT1166_EXAMPLE_SOURCES)
        get_filename_component(example_name "${example_source}" NAME_WE)
        set(example_target "ads1299_rt1166_example_${example_name}")
        add_library(${example_target} OBJECT "${example_source}")
        target_compile_features(${example_target} PRIVATE c_std_11)
        target_include_directories(${example_target} PRIVATE
            ../../../core_driver/ads1299
            ads1299_port
            mcuxpresso_adapter
            board
            examples)
        target_link_libraries(${example_target} PRIVATE
            ads1299_rt1166
            ads1299_official_sdk)
        list(APPEND ADS1299_RT1166_EXAMPLE_TARGETS ${example_target})
    endforeach()

    # Stable aggregate target for CI/parent projects. Building this target
    # compiles all six examples while keeping their main() functions isolated.
    add_custom_target(ads1299_rt1166_examples_smoke ALL
        DEPENDS ${ADS1299_RT1166_EXAMPLE_TARGETS})
endif()
