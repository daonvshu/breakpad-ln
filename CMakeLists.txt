cmake_minimum_required(VERSION 3.12)

project(crashlistener)

set(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

add_library(${PROJECT_NAME}
    crashlistener.h
    crashlistener.cpp
)

add_library(bugreport::lib ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
  # set library path
  include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/includes/windows)
  target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows)

  # defined BUGREPORT_EXECUTABLE
  add_compile_definitions(BUGREPORT_EXECUTABLE="bugreport.exe")

  # add pdb symbol for release
  if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
  endif()

  # link debug version
  if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(OUTPUT_TYPE_SUFFIX "d")
  endif()

  # x86/x64
  if (CMAKE_CL_64)
    set(OUTPUT_CL_SUFFIX "_x64")
  else()
    set(OUTPUT_CL_SUFFIX "_x86")
  endif()

  # link breakpad target version
  target_link_libraries(${PROJECT_NAME} PRIVATE breakpad_client${OUTPUT_TYPE_SUFFIX}${OUTPUT_CL_SUFFIX}.lib)

else()
  # set library path
  include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/includes/linux)
  # defined BUGREPORT_EXECUTABLE
  add_compile_definitions(BUGREPORT_EXECUTABLE="./bugreport")

  # check compiler type
  if (${CMAKE_C_COMPILER} MATCHES "arm-linux-gnueabihf-gcc")
    # check compiler version
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (${GCC_VERSION} GREATER_EQUAL 7)
      set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/arm64-7.3.1)
    elseif (${GCC_VERSION} GREATER_EQUAL 6)
      set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/arm64-6.2.1)
    else()
      set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/arm64-4.9.4)
    endif ()
  elseif (${CMAKE_C_COMPILER} MATCHES "aarch64-linux-gnu-gcc")
    set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/aarch64-9.3)
  else ()
    message(FATAL_ERROR "Cannot link library on target linux!")
  endif ()
  # add link directory with target gcc version
  target_link_directories(${PROJECT_NAME} PUBLIC ${LIB_DIR})

  # link breakpad
  target_link_libraries(${PROJECT_NAME} PRIVATE breakpad_client_linux pthread)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)