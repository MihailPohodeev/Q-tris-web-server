cmake_minimum_required(VERSION 3.20)
project(Q-Ttis-web LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)  # Определение для препроцессора
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra")
endif()

find_package(Boost 1.66 REQUIRED COMPONENTS system)

file(GLOB_RECURSE SOURCES "src/*.cxx")
file(GLOB_RECURSE HEADERS "include/*.hxx")

add_executable(${PROJECT_NAME}
	${SOURCES}
	${HEADERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
	${Boost_INCLUDE_DIRS}
	include/
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Boost::boost
	Boost::system
)

if(UNIX AND NOT APPLE)
	target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()
