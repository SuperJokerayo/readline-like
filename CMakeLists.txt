cmake_minimum_required(VERSION 3.10)

project(Readline-like VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(main ./src/main.cpp ./src/readline.cpp ./src/readline_state.cpp)
