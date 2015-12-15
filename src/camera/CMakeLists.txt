cmake_minimum_required(VERSION 2.8)

add_library(camera STATIC fb.c)

target_include_directories(camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
