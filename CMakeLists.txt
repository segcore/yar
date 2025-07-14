cmake_minimum_required(VERSION 3.0)
project(yar VERSION 1.0 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS On)
add_library(yar INTERFACE) # To just get the includes
add_library(yar_impl OBJECT yar.c) # To also get implementation
target_include_directories(yar INTERFACE ".")
target_include_directories(yar_impl PUBLIC ".")
add_subdirectory(examples)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
