cmake_minimum_required(VERSION 2.8)

add_library(sensor SHARED sensor_client.c sensor_process.c)

target_include_directories(sensor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
