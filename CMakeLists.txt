cmake_minimum_required (VERSION 2.8)
project (esg)

find_package (Boost REQUIRED COMPONENTS
	system)
include_directories (
	include/
	${Boost_INCLUDE_DIRS})

add_executable (ggd
	src/gg_server.cpp
	src/gg_connection.cpp
	src/main.cpp)
target_link_libraries (ggd
	${Boost_LIBRARIES})