cmake_minimum_required(VERSION 3.12)

project(crashlistener)

set(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

if (WIN32)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes/windows)
add_compile_definitions(BUGREPORT_EXECUTABLE="bugreport.exe")
else()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes/linux)
add_compile_definitions(BUGREPORT_EXECUTABLE="./bugreport")
endif()

add_library(${PROJECT_NAME} 
    crashlistener.h
    crashlistener.cpp
)

add_library(bugreport::lib ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
)