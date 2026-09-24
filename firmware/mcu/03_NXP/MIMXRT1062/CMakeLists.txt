# Include from an existing official SDK Cortex-M7 application. This is a
# library integration target, not a standalone board/startup/linker project.
add_library(ads1299_rt1062 STATIC
    ../../../core_driver/ads1299/ads1299.c
    ../../../core_driver/ads1299/ads1299_model.c
    ../../../core_driver/ads1299/ads1299_frame.c
    ../../../core_driver/ads1299/ads1299_multi.c
    ads1299_port/ads1299_spi.c
    ads1299_port/ads1299_gpio.c
    ads1299_port/ads1299_drdy.c
    mcuxpresso_adapter/ads1299_mcuxpresso_hal.c
    mcuxpresso_adapter/ads1299_mcuxpresso_init.c)
target_compile_features(ads1299_rt1062 PUBLIC c_std_11)
target_include_directories(ads1299_rt1062 PUBLIC
    ../../../core_driver/ads1299 ads1299_port mcuxpresso_adapter)
# The parent SDK application must supply its SDK include/compile usage target.
# Never silently fall back to mock headers or a host compiler.
if(NOT TARGET ads1299_official_sdk)
    message(FATAL_ERROR "Define an ads1299_official_sdk INTERFACE target with official SDK includes, CPU and compiler options before add_subdirectory")
endif()
target_link_libraries(ads1299_rt1062 PUBLIC ads1299_official_sdk)

# Optional reference-board integration. Keep vendor startup/linker/generated
# sources in the parent official MCUXpresso SDK application; this target only
# adds the ADS1299 EVKB binding to the shared driver library. The compatibility
# evkb_reference.c entry point remains available while the explicit binding is
# the preferred single-config path for new applications.
option(ADS1299_RT1062_ENABLE_EVKB "Build the MIMXRT1062 EVKB ADS1299 board binding" OFF)
if(ADS1299_RT1062_ENABLE_EVKB)
    target_sources(ads1299_rt1062 PRIVATE
        board/evkb_reference.c
        board/board_ads1299_binding.c)
    target_include_directories(ads1299_rt1062 PUBLIC board)
endif()

# Compile-only contract checks are deliberately opt-in. They catch board and
# adapter API drift while still requiring the real official SDK target above.
option(ADS1299_RT1062_COMPILE_SMOKE "Compile MIMXRT1062 ADS1299 integration contract checks" OFF)
if(ADS1299_RT1062_COMPILE_SMOKE)
    if(NOT ADS1299_RT1062_ENABLE_EVKB)
        message(FATAL_ERROR "ADS1299_RT1062_COMPILE_SMOKE requires ADS1299_RT1062_ENABLE_EVKB=ON")
    endif()
    add_library(ads1299_rt1062_compile_smoke OBJECT
        board/board_ads1299_config_check.c
        mcuxpresso_adapter/compile_smoke.c
        tests/test_board_binding_contract.c)
    target_compile_features(ads1299_rt1062_compile_smoke PRIVATE c_std_11)
    target_include_directories(ads1299_rt1062_compile_smoke PRIVATE
        ../../../core_driver/ads1299 ads1299_port mcuxpresso_adapter board)
    target_link_libraries(ads1299_rt1062_compile_smoke PRIVATE
        ads1299_rt1062 ads1299_official_sdk)
endif()

# Optional beginner-example build contract. This compiles the progressive
# probe -> internal-test -> input-short -> 250-SPS EEG -> bounded-stream flows
# against the same shared core and real MCUXpresso usage requirements. It is
# intentionally separate from board validation and provides no startup/main.
option(ADS1299_RT1062_EXAMPLE_SMOKE "Compile MIMXRT1062 progressive ADS1299 examples" OFF)
if(ADS1299_RT1062_EXAMPLE_SMOKE)
    if(NOT ADS1299_RT1062_ENABLE_EVKB)
        message(FATAL_ERROR "ADS1299_RT1062_EXAMPLE_SMOKE requires ADS1299_RT1062_ENABLE_EVKB=ON")
    endif()
    add_library(ads1299_rt1062_example_smoke STATIC
        examples/probe_id.c
        examples/internal_test.c
        examples/input_short.c
        examples/eeg_250sps.c
        examples/bounded_stream.c
        examples/example_contract_smoke.c)
    target_compile_features(ads1299_rt1062_example_smoke PRIVATE c_std_11)
    target_include_directories(ads1299_rt1062_example_smoke PRIVATE
        ../../../core_driver/ads1299 ads1299_port mcuxpresso_adapter board examples)
    target_link_libraries(ads1299_rt1062_example_smoke PRIVATE
        ads1299_rt1062 ads1299_official_sdk)

    # Keep the public bounded-stream handoff contract in the same build graph as
    # the examples. This catches API/frame-shape drift without executing target
    # hardware or introducing a second ADS1299 frame representation.
    add_library(ads1299_rt1062_stream_contract_smoke OBJECT
        tests/test_bounded_stream_contract.c)
    target_compile_features(ads1299_rt1062_stream_contract_smoke PRIVATE c_std_11)
    target_include_directories(ads1299_rt1062_stream_contract_smoke PRIVATE
        ../../../core_driver/ads1299 examples)
    target_link_libraries(ads1299_rt1062_stream_contract_smoke PRIVATE
        ads1299_rt1062_example_smoke ads1299_official_sdk)
endif()
