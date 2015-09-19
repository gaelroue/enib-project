cmake_minimum_required(VERSION 2.8)

set(PRJ elec)
set(MAIN src/main.c)

set(TEST test/serial.c)

project($PRJ)

include_directories(
	./include
)

add_subdirectory(src/drivers)

add_executable(${PRJ} ${MAIN})
#add_executable(${PRJ} ${TEST})
target_link_libraries(elec drivers)
