cmake_minimum_required(VERSION 2.8)

add_library(websocket STATIC socket_function.c serveur_socket.c)

target_link_libraries(websocket drivers)
target_include_directories(websocket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
