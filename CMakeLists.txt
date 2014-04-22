cmake_minimum_required (VERSION 2.8)
project (esg)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package (Boost REQUIRED COMPONENTS
	system)
find_package (Libgadu REQUIRED)
add_subdirectory (deps EXCLUDE_FROM_ALL)
include_directories (
	include/
	deps/sqlite3/
	${Boost_INCLUDE_DIRS}
	${LIBGADU_INCLUDE_DIR})
add_definitions (${LIBGADU_DEFINITIONS})
add_executable (ggd
	src/gg_server.cpp
	src/gg_connection.cpp
	src/main.cpp
	src/database.cpp
	src/connection_manager.cpp)
target_link_libraries (ggd
	sqlite3
	${Boost_LIBRARIES}
	${LIBGADU_LIBRARIES})