# Include from an existing official SDK Cortex-M7 application. This is a
# library integration target, not a standalone board/startup/linker project.
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
# The parent SDK application must supply its SDK include/compile usage target.
# Never silently fall back to mock headers or a host compiler.
if(NOT TARGET ads1299_official_sdk)
    message(FATAL_ERROR "Define an ads1299_official_sdk INTERFACE target with official SDK includes, CPU and compiler options before add_subdirectory")
endif()
target_link_libraries(ads1299_rt1170 PUBLIC ads1299_official_sdk)
