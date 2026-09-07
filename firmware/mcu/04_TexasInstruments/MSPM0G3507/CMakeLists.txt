# Thin SDK integration target; vendor includes/options supplied by the parent.
if(NOT TARGET ads1299_official_sdk)
    message(FATAL_ERROR "Define ads1299_official_sdk with the installed TI SDK and ARM compiler options")
endif()
add_library(ads1299_mspm0 STATIC
    ../../../core_driver/ads1299/ads1299.c
    ../../../core_driver/ads1299/ads1299_model.c
    ../../../core_driver/ads1299/ads1299_frame.c
    ../../../core_driver/ads1299/ads1299_multi.c
    ads1299_port/ads1299_spi.c ads1299_port/ads1299_gpio.c
    ads1299_port/ads1299_drdy.c driverlib_adapter/ads1299_mspm0_hal.c)
target_compile_features(ads1299_mspm0 PUBLIC c_std_11)
target_include_directories(ads1299_mspm0 PUBLIC
    ../../../core_driver/ads1299 ads1299_port driverlib_adapter)
target_link_libraries(ads1299_mspm0 PUBLIC ads1299_official_sdk)
