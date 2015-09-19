cmake_minimum_required(VERSION 2.8)

add_library(drivers SHARED xbee_serial.c)

target_include_directories(drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
