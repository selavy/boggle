cmake_minimum_required(VERSION 2.8.11)

project(lesschess C)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
set(CMAME_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_BUILD_TYPE Release)

add_compile_options("--std=gnu11")
add_compile_options("-Wall")
add_compile_options("-Werror")

add_compile_options("-Wno-unused-function")
add_compile_options("-Wno-unused-but-set-variable")

add_executable(boggle main.c)
