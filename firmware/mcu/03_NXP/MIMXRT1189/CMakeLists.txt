# MIMXRT1189 ADS1299 integration for an existing official MCUXpresso SDK app.
# This is intentionally not a standalone startup/linker/board project.
# The consuming vendor project owns startup, linker, clocks, pinmux and the
# concrete board_ads1299_hal() implementation.
if(NOT TARGET ads1299_official_sdk)
    message(FATAL_ERROR "Define an ads1299_official_sdk INTERFACE target with official MCUXpresso SDK includes, CPU/compiler options and board dependencies before add_subdirectory")
endif()

add_library(ads1299_rt1189 STATIC
    ../../../core_driver/ads1299/ads1299.c
    ../../../core_driver/ads1299/ads1299_model.c
    ../../../core_driver/ads1299/ads1299_frame.c
    ../../../core_driver/ads1299/ads1299_multi.c
    ads1299_port/ads1299_spi.c
    ads1299_port/ads1299_gpio.c
    ads1299_port/ads1299_drdy.c)

target_compile_features(ads1299_rt1189 PUBLIC c_std_11)
target_include_directories(ads1299_rt1189 PUBLIC
    ../../../core_driver/ads1299
    ads1299_port
    board)
target_link_libraries(ads1299_rt1189 PUBLIC ads1299_official_sdk)

# Compile the board-facing API boundary. This checks that the model folder and
# a consuming project agree on board_ads1299_hal() without inventing EVK pins.
# It does not link a concrete board binding and therefore is not board evidence.
option(ADS1299_RT1189_BOARD_CONTRACT_SMOKE
       "Compile the MIMXRT1189 ADS1299 board API contract" OFF)
if(ADS1299_RT1189_BOARD_CONTRACT_SMOKE)
    add_library(ads1299_rt1189_board_contract OBJECT
        board/board_ads1299_binding_check.c)
    target_compile_features(ads1299_rt1189_board_contract PRIVATE c_std_11)
    target_include_directories(ads1299_rt1189_board_contract PRIVATE
        ../../../core_driver/ads1299
        ads1299_port
        board)
    target_link_libraries(ads1299_rt1189_board_contract PRIVATE
        ads1299_rt1189
        ads1299_official_sdk)

    add_custom_target(ads1299_rt1189_board_smoke ALL
        DEPENDS ads1299_rt1189 ads1299_rt1189_board_contract)
endif()

# Compile each progressive example independently against the same shared core.
# The examples intentionally own main(), so OBJECT targets catch API/include
# regressions without pretending to create flashable vendor firmware images.
option(ADS1299_RT1189_EXAMPLE_SMOKE
       "Compile all MIMXRT1189 ADS1299 progressive examples" OFF)
if(ADS1299_RT1189_EXAMPLE_SMOKE)
    set(ADS1299_RT1189_EXAMPLE_SOURCES
        examples/main_ads1299.c
        examples/probe_id.c
        examples/internal_test.c
        examples/input_short.c
        examples/eeg_250sps.c
        examples/stream_bounded.c)

    set(ADS1299_RT1189_EXAMPLE_TARGETS "")
    foreach(example_source IN LISTS ADS1299_RT1189_EXAMPLE_SOURCES)
        get_filename_component(example_name "${example_source}" NAME_WE)
        set(example_target "ads1299_rt1189_example_${example_name}")
        add_library(${example_target} OBJECT "${example_source}")
        target_compile_features(${example_target} PRIVATE c_std_11)
        target_include_directories(${example_target} PRIVATE
            ../../../core_driver/ads1299
            ads1299_port
            board
            examples)
        target_link_libraries(${example_target} PRIVATE
            ads1299_rt1189
            ads1299_official_sdk)
        list(APPEND ADS1299_RT1189_EXAMPLE_TARGETS ${example_target})
    endforeach()

    add_custom_target(ads1299_rt1189_examples_smoke ALL
        DEPENDS ${ADS1299_RT1189_EXAMPLE_TARGETS})
endif()

# Compile the platform tests with the exact headers/options used by the official
# SDK integration. These are OBJECT targets on purpose: this gate catches API,
# include and warning regressions without claiming that an embedded executable
# was linked, flashed or run on MIMXRT1180-EVK hardware.
#
# test_drdy.c currently owns its own main(), while test_main.c is the runner for
# the SPI/reset/frame/register tests. Keeping them in separate OBJECT targets
# preserves both existing test entry-point styles without deleting user work.
option(ADS1299_RT1189_PLATFORM_TEST_SMOKE
       "Compile MIMXRT1189 ADS1299 platform tests against the official SDK contract" OFF)
if(ADS1299_RT1189_PLATFORM_TEST_SMOKE)
    add_library(ads1299_rt1189_tests_core OBJECT
        tests/test_main.c
        tests/test_spi.c
        tests/test_reset.c
        tests/test_frame.c
        tests/test_register.c)
    add_library(ads1299_rt1189_test_drdy OBJECT
        tests/test_drdy.c)

    foreach(test_target IN ITEMS
            ads1299_rt1189_tests_core
            ads1299_rt1189_test_drdy)
        target_compile_features(${test_target} PRIVATE c_std_11)
        target_include_directories(${test_target} PRIVATE
            ../../../core_driver/ads1299
            ads1299_port
            board
            tests)
        target_link_libraries(${test_target} PRIVATE
            ads1299_rt1189
            ads1299_official_sdk)
    endforeach()

    add_custom_target(ads1299_rt1189_platform_tests_smoke ALL
        DEPENDS
            ads1299_rt1189
            ads1299_rt1189_tests_core
            ads1299_rt1189_test_drdy)
endif()
