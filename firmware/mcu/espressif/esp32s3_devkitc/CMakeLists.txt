cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/../../../core_driver/ads1299"
    "${CMAKE_CURRENT_LIST_DIR}/../../../common/data_packet"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ads1299_core_esp32s3)
