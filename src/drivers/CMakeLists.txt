cmake_minimum_required(VERSION 2.8)

add_library(drivers STATIC xbee_serial.c timer.c semaphore.c)


target_include_directories(drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
