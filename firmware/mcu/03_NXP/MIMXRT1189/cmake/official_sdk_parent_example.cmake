# MIMXRT1189 ADS1299 parent-project integration example.
#
# Include this file only from an existing official MCUXpresso SDK application
# for the MIMXRT1189 family. It intentionally does NOT invent startup files,
# linker scripts, pinmux, clock roots, CPU flags, or physical ADS1299 wiring.
# The generated/vendor parent project owns those details.
#
# Required parent-project inputs:
#   ADS1299_RT1189_SDK_INCLUDE_DIRS  - official SDK include directories
#   ADS1299_RT1189_SDK_LIBRARIES     - existing SDK/board targets or libraries
# Optional inputs:
#   ADS1299_RT1189_SDK_COMPILE_OPTIONS - generated CPU/compiler options when
#                                        not propagated by SDK targets
#   ADS1299_RT1189_BOARD_DEFINITIONS - board/config compile definitions
#
# A concrete board_ads1299_hal() implementation remains the application's
# board-level responsibility. The smoke targets below compile the board API
# boundary and progressive examples; they do not claim verified EVK wiring.

if(NOT DEFINED ADS1299_RT1189_SDK_INCLUDE_DIRS)
    message(FATAL_ERROR "Set ADS1299_RT1189_SDK_INCLUDE_DIRS from the official MCUXpresso project")
endif()

if(NOT DEFINED ADS1299_RT1189_SDK_LIBRARIES)
    message(FATAL_ERROR "Set ADS1299_RT1189_SDK_LIBRARIES to existing official SDK/board targets or libraries")
endif()

if(TARGET ads1299_official_sdk)
    message(FATAL_ERROR "ads1299_official_sdk already exists; do not include official_sdk_parent_example.cmake twice")
endif()

add_library(ads1299_official_sdk INTERFACE)
target_include_directories(ads1299_official_sdk INTERFACE
    ${ADS1299_RT1189_SDK_INCLUDE_DIRS})
target_link_libraries(ads1299_official_sdk INTERFACE
    ${ADS1299_RT1189_SDK_LIBRARIES})

if(DEFINED ADS1299_RT1189_SDK_COMPILE_OPTIONS)
    target_compile_options(ads1299_official_sdk INTERFACE
        ${ADS1299_RT1189_SDK_COMPILE_OPTIONS})
endif()

if(DEFINED ADS1299_RT1189_BOARD_DEFINITIONS)
    target_compile_definitions(ads1299_official_sdk INTERFACE
        ${ADS1299_RT1189_BOARD_DEFINITIONS})
endif()

# Exercise both compile-only layers against the real parent SDK. Neither target
# creates a flashable image or supplies a board binding, so success is software
# build evidence only and must not be reported as BOARD-VERIFIED.
set(ADS1299_RT1189_BOARD_CONTRACT_SMOKE ON CACHE BOOL "" FORCE)
set(ADS1299_RT1189_EXAMPLE_SMOKE ON CACHE BOOL "" FORCE)

# Usage from the parent CMakeLists.txt after setting the variables above:
#   include(<repo>/firmware/mcu/03_NXP/MIMXRT1189/cmake/official_sdk_parent_example.cmake)
#   add_subdirectory(<repo>/firmware/mcu/03_NXP/MIMXRT1189 ads1299_rt1189)
#   cmake --build <build-dir> --target ads1299_rt1189_board_smoke
#   cmake --build <build-dir> --target ads1299_rt1189_examples_smoke
#
# Record the exact SDK version, board/project, compiler and successful commands
# before promoting this leaf from TEMPLATE to BUILD-VERIFIED.
