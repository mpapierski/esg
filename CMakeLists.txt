cmake_minimum_required (VERSION 2.8)
project (esg)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package (Boost REQUIRED COMPONENTS
	system)
find_package (Libgadu REQUIRED)
find_package (Sqlite3 REQUIRED)
include_directories (
	include/
	${Boost_INCLUDE_DIRS}
	${LIBGADU_INCLUDE_DIR}
	${SQLITE3_INCLUDE_DIR})
add_definitions (${LIBGADU_DEFINITIONS})
add_executable (ggd
	src/gg_server.cpp
	src/gg_connection.cpp
	src/main.cpp
	src/database.cpp)
target_link_libraries (ggd
	${Boost_LIBRARIES}
	${LIBGADU_LIBRARIES}
	${SQLITE3_LIBRARIES})