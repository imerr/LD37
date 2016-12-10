cmake_minimum_required(VERSION 2.8.4)
project(LD37)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(engine ${CMAKE_CURRENT_BINARY_DIR}/engine)
include("engine/includes.cmake")
include("engine/compiler.cmake")
include_directories(.)

set(SOURCE_FILES
    src/main.cpp src/LD37.cpp src/LD37.hpp src/Room.cpp src/Room.hpp)

if (ANDROID)
	add_library(LD37 SHARED ${SOURCE_FILES})
else()
	add_executable(LD37 ${SOURCE_FILES})
endif()

target_link_libraries(LD37 Engine)
if (WIN32)
    set_target_properties(LD37 PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
    set_target_properties(LD37 PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
    set_target_properties(LD37 PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(LD37 PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif()