if(NOT TARGET ads1299_official_sdk)
    message(FATAL_ERROR "Provide ads1299_official_sdk with real G474/CMSIS headers and CPU options")
endif()
add_library(ads1299_g4 STATIC
    ../../../core_driver/ads1299/ads1299.c
    ../../../core_driver/ads1299/ads1299_model.c
    ../../../core_driver/ads1299/ads1299_frame.c
    ../../../core_driver/ads1299/ads1299_multi.c
    ads1299_port/ads1299_spi.c ads1299_port/ads1299_gpio.c
    ads1299_port/ads1299_drdy.c cmsis_adapter/ads1299_g4_hal.c)
target_compile_features(ads1299_g4 PUBLIC c_std_11)
target_include_directories(ads1299_g4 PUBLIC ../../../core_driver/ads1299 ads1299_port cmsis_adapter)
target_link_libraries(ads1299_g4 PUBLIC ads1299_official_sdk)
