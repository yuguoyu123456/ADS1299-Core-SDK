# MIMXRT1166 ADS1299 parent-project integration example.
#
# Include this file only from an existing, generated official MCUXpresso SDK
# application. It intentionally does NOT invent startup files, linker scripts,
# pinmux, clock roots, CPU flags, or board pins. The parent SDK project owns all
# of those details.
#
# Before including this file, the parent project must provide:
#   ADS1299_RT1166_SDK_INCLUDE_DIRS  - official SDK include directories
#   ADS1299_RT1166_SDK_LIBRARIES     - existing SDK/board targets or libraries
#   ADS1299_BOARD_*                  - board macros required by
#                                     board/board_ads1299_config.h, normally as
#                                     compile definitions on the application
#                                     or ads1299_official_sdk target.
#
# This creates the exact target name required by ../CMakeLists.txt and enables
# the board/config compile smoke plus all progressive example compile checks.

if(NOT DEFINED ADS1299_RT1166_SDK_INCLUDE_DIRS)
    message(FATAL_ERROR "Set ADS1299_RT1166_SDK_INCLUDE_DIRS from the generated official MCUXpresso project")
endif()

if(NOT DEFINED ADS1299_RT1166_SDK_LIBRARIES)
    message(FATAL_ERROR "Set ADS1299_RT1166_SDK_LIBRARIES to existing official SDK/board targets or libraries")
endif()

if(TARGET ads1299_official_sdk)
    message(FATAL_ERROR "ads1299_official_sdk already exists; do not include official_sdk_parent_example.cmake twice")
endif()

add_library(ads1299_official_sdk INTERFACE)
target_include_directories(ads1299_official_sdk INTERFACE
    ${ADS1299_RT1166_SDK_INCLUDE_DIRS})
target_link_libraries(ads1299_official_sdk INTERFACE
    ${ADS1299_RT1166_SDK_LIBRARIES})

# The consumer may append its generated CPU/compiler options here when those
# options are not already propagated by ADS1299_RT1166_SDK_LIBRARIES. Keeping
# them in the parent project avoids pretending that one flag set fits every
# MCUXpresso SDK/toolchain configuration.
if(DEFINED ADS1299_RT1166_SDK_COMPILE_OPTIONS)
    target_compile_options(ads1299_official_sdk INTERFACE
        ${ADS1299_RT1166_SDK_COMPILE_OPTIONS})
endif()

# Hardware macros belong to the board project. Supplying them as a list keeps
# the ADS1299 leaf free of guessed pins. Example list syntax (names only):
#   ADS1299_BOARD_LPSPI_BASE=<generated SDK expression>
#   ADS1299_BOARD_LPSPI_SOURCE_CLOCK_HZ=<generated clock expression>
#   ADS1299_BOARD_CS_GPIO=... ADS1299_BOARD_CS_PIN=...
#   ADS1299_BOARD_RESET_GPIO=... ADS1299_BOARD_RESET_PIN=...
#   ADS1299_BOARD_PWDN_GPIO=... ADS1299_BOARD_PWDN_PIN=...
#   ADS1299_BOARD_START_GPIO=... ADS1299_BOARD_START_PIN=...
#   ADS1299_BOARD_DRDY_GPIO=... ADS1299_BOARD_DRDY_PIN=...
if(DEFINED ADS1299_RT1166_BOARD_DEFINITIONS)
    target_compile_definitions(ads1299_official_sdk INTERFACE
        ${ADS1299_RT1166_BOARD_DEFINITIONS})
endif()

set(ADS1299_RT1166_ENABLE_BOARD_BINDING ON CACHE BOOL "" FORCE)
set(ADS1299_RT1166_COMPILE_SMOKE ON CACHE BOOL "" FORCE)
set(ADS1299_RT1166_EXAMPLE_SMOKE ON CACHE BOOL "" FORCE)

# Usage from the parent CMakeLists.txt after setting the variables above:
#   include(<repo>/firmware/mcu/03_NXP/MIMXRT1166/cmake/official_sdk_parent_example.cmake)
#   add_subdirectory(<repo>/firmware/mcu/03_NXP/MIMXRT1166 ads1299_rt1166)
#   cmake --build <build-dir> --target ads1299_rt1166_compile_smoke
#   cmake --build <build-dir> --target ads1299_rt1166_examples_smoke
#
# Successful compilation is BUILD evidence only. It does not establish
# BOARD-VERIFIED status or prove electrical/hardware behavior.
