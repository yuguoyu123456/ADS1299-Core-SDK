cmake_minimum_required(VERSION 3.16)
project(ads1299_rt1189_host_smoke C)

# Host-only compile harness for the portable MIMXRT1189 integration boundary.
# This intentionally does NOT emulate MCUXpresso, startup, linker, clocks,
# pinmux, LPSPI hardware, or the physical MIMXRT1180-EVK.  It gives the leaf a
# reproducible way to catch ordinary C/API/include regressions before an
# official SDK build is available.
add_library(ads1299_official_sdk INTERFACE)

set(ADS1299_RT1189_BOARD_CONTRACT_SMOKE ON CACHE BOOL "" FORCE)
set(ADS1299_RT1189_EXAMPLE_SMOKE ON CACHE BOOL "" FORCE)
set(ADS1299_RT1189_PLATFORM_TEST_SMOKE ON CACHE BOOL "" FORCE)

add_subdirectory(../.. rt1189)

add_custom_target(rt1189_host_smoke ALL
    DEPENDS
        ads1299_rt1189_board_smoke
        ads1299_rt1189_examples_smoke
        ads1299_rt1189_platform_tests_smoke)
