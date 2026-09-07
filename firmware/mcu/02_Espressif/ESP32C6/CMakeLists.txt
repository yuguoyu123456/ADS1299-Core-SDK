cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../core_driver/ads1299")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ads1299_esp32c6_reference)
