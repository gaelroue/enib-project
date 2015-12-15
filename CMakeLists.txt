cmake_minimum_required(VERSION 2.8)

set(PRJ elec)
set(MAIN src/main.c)

set(CMAKE_BUILD_TYPE Debug)



#set(TEST test/serial.c)
#set(TEST test/thread.c)
#set(TEST test/server.c)
#set(TEST test/send.c)
#set(TEST test/send2.c)
#set(TEST test/sensor.c)
#set(TEST test/server.c)
#set(TEST test/time.c)
#set(TEST test/semaphore_1.c)
#set(TEST test/semaphore_2.c)
set(TEST test/websocket.c)

project($PRJ)

include_directories(
	./include
)

add_subdirectory(src/drivers)
add_subdirectory(src/zigbee)
add_subdirectory(src/sensor)
add_subdirectory(src/websocket)

#add_executable(${PRJ} ${MAIN})
add_executable(${PRJ} ${TEST})
target_link_libraries(${PRJ} drivers zigbee sensor -lm pthread websocket)


