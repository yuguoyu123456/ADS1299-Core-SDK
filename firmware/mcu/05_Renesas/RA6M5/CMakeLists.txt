# Integration library only: the consuming FSP project owns BSP and generated code.
if(NOT TARGET ads1299_official_sdk)
    message(FATAL_ERROR "Supply ads1299_official_sdk with real FSP and generated BSP include/CPU options")
endif()
add_library(ads1299_ra6m5 STATIC
    ../../../core_driver/ads1299/ads1299.c
    ../../../core_driver/ads1299/ads1299_model.c
    ../../../core_driver/ads1299/ads1299_frame.c
    ../../../core_driver/ads1299/ads1299_multi.c
    ads1299_port/ads1299_spi.c
    ads1299_port/ads1299_gpio.c
    ads1299_port/ads1299_drdy.c
    fsp_adapter/ads1299_fsp_hal.c)
target_compile_features(ads1299_ra6m5 PUBLIC c_std_11)
target_include_directories(ads1299_ra6m5 PUBLIC
    ../../../core_driver/ads1299 ads1299_port fsp_adapter)
target_link_libraries(ads1299_ra6m5 PUBLIC ads1299_official_sdk)
