# MIMXRT1170 ADS1299 parent-project integration example.
#
# Include this file only from an existing, generated official MCUXpresso SDK
# Cortex-M7 application for the MIMXRT1170 family. It intentionally does NOT
# invent startup files, linker scripts, pinmux, clock roots, CPU flags, or
# physical ADS1299 wiring. The parent SDK project owns those details.
#
# Required parent-project inputs:
#   ADS1299_RT1170_SDK_INCLUDE_DIRS  - official SDK include directories
#   ADS1299_RT1170_SDK_LIBRARIES     - existing SDK/board targets or libraries
# Optional inputs:
#   ADS1299_RT1170_SDK_COMPILE_OPTIONS - generated CPU/compiler options when
#                                        they are not propagated by libraries
#   ADS1299_RT1170_BOARD_DEFINITIONS - board-specific compile definitions used
#                                      by the selected board binding.
#
# The RT1170 leaf currently keeps its EVK reference mapping explicitly opt-in
# and documents it as reference-only rather than BOARD-VERIFIED wiring. This
# scaffold therefore enables that binding only for compile-contract validation;
# a real application should replace/confirm the mapping from its schematic and
# generated pin/clock configuration before hardware use.

if(NOT DEFINED ADS1299_RT1170_SDK_INCLUDE_DIRS)
    message(FATAL_ERROR "Set ADS1299_RT1170_SDK_INCLUDE_DIRS from the generated official MCUXpresso project")
endif()

if(NOT DEFINED ADS1299_RT1170_SDK_LIBRARIES)
    message(FATAL_ERROR "Set ADS1299_RT1170_SDK_LIBRARIES to existing official SDK/board targets or libraries")
endif()

if(TARGET ads1299_official_sdk)
    message(FATAL_ERROR "ads1299_official_sdk already exists; do not include official_sdk_parent_example.cmake twice")
endif()

add_library(ads1299_official_sdk INTERFACE)
target_include_directories(ads1299_official_sdk INTERFACE
    ${ADS1299_RT1170_SDK_INCLUDE_DIRS})
target_link_libraries(ads1299_official_sdk INTERFACE
    ${ADS1299_RT1170_SDK_LIBRARIES})

if(DEFINED ADS1299_RT1170_SDK_COMPILE_OPTIONS)
    target_compile_options(ads1299_official_sdk INTERFACE
        ${ADS1299_RT1170_SDK_COMPILE_OPTIONS})
endif()

if(DEFINED ADS1299_RT1170_BOARD_DEFINITIONS)
    target_compile_definitions(ads1299_official_sdk INTERFACE
        ${ADS1299_RT1170_BOARD_DEFINITIONS})
endif()

# Exercise both layers of the leaf's build contract against the real parent
# SDK: board/platform binding and every progressive example. These options do
# not create a standalone firmware image and do not imply hardware validation.
set(ADS1299_RT1170_ENABLE_EVK_REFERENCE ON CACHE BOOL "" FORCE)
set(ADS1299_RT1170_COMPILE_SMOKE ON CACHE BOOL "" FORCE)
set(ADS1299_RT1170_EXAMPLE_SMOKE ON CACHE BOOL "" FORCE)

# Usage from the generated parent CMakeLists.txt after setting the variables:
#   include(<repo>/firmware/mcu/03_NXP/MIMXRT1170/cmake/official_sdk_parent_example.cmake)
#   add_subdirectory(<repo>/firmware/mcu/03_NXP/MIMXRT1170 ads1299_rt1170)
#   cmake --build <build-dir> --target ads1299_rt1170_compile_smoke
#   cmake --build <build-dir> --target ads1299_rt1170_examples_smoke
#
# A successful compile is BUILD evidence only. Do not change the leaf to
# BOARD-VERIFIED until the stated physical board and ADS1299 wiring are actually
# exercised and evidence is recorded.
