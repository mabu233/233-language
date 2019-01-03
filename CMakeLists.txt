cmake_minimum_required(VERSION 3.13)
project(233_language)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_definitions(-DLANG233_DEBUG)

add_executable(233lang src/233_main.cc src/233_parser.cc src/233_scanner.cc src/233_vm.cc src/233_jit.cc)