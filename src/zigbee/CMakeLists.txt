cmake_minimum_required(VERSION 2.8)

add_library(zigbee STATIC xbee_client.c xbee_server.c)

target_link_libraries(zigbee drivers)
target_include_directories(zigbee PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
