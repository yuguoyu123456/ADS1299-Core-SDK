# ESP-IDF keeps the historical component integration path unchanged.
if(COMMAND idf_component_register)
    idf_component_register(
        SRCS
            "ads1299.c"
            "ads1299_frame.c"
            "ads1299_multi.c"
            "ads1299_model.c"
        INCLUDE_DIRS "."
    )
else()
    # Standalone host build for portable compile checks and offline smoke tests.
    cmake_minimum_required(VERSION 3.16)
    project(ads1299_core_driver C)

    add_library(ads1299_core STATIC
        ads1299.c
        ads1299_frame.c
        ads1299_multi.c
        ads1299_model.c
    )
    target_include_directories(ads1299_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
    target_compile_features(ads1299_core PUBLIC c_std_99)

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(ads1299_core PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    option(ADS1299_BUILD_HOST_TESTS "Build ADS1299 offline core smoke test" ON)
    if(ADS1299_BUILD_HOST_TESTS)
        enable_testing()
        add_executable(ads1299_core_selftest ads1299_core_selftest.c)
        target_link_libraries(ads1299_core_selftest PRIVATE ads1299_core)
        if(UNIX AND NOT APPLE)
            target_link_libraries(ads1299_core_selftest PRIVATE m)
        endif()
        if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(ads1299_core_selftest PRIVATE -Wall -Wextra -Wpedantic)
        endif()
        add_test(NAME ads1299_core_selftest COMMAND ads1299_core_selftest)
    endif()
endif()
