if(COMMAND idf_component_register)
    idf_component_register(
        SRCS "ads1299_transport.c"
        INCLUDE_DIRS "." "../common/data_packet"
        REQUIRES data_packet
    )
else()
    add_library(ads1299_transport STATIC ads1299_transport.c)
    target_include_directories(ads1299_transport PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../common/data_packet
    )
endif()
