idf_component_register(
    SRCS
        "app_main.c"
        "../../../ads1299_port/ads1299_spi.c"
        "../../../ads1299_port/ads1299_gpio.c"
        "../../../ads1299_port/ads1299_drdy.c"
        "../../../ads1299_port/ads1299_espidf_hal.c"
        "../../../../../../core_driver/ads1299/ads1299.c"
        "../../../../../../core_driver/ads1299/ads1299_frame.c"
        "../../../../../../core_driver/ads1299/ads1299_model.c"
        "../../../../../../core_driver/ads1299/ads1299_multi.c"
    INCLUDE_DIRS
        "."
        "../../../ads1299_port"
        "../../../board"
        "../../../../../../core_driver/ads1299"
    REQUIRES driver
)
