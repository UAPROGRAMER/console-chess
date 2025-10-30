cmake_minimum_required(VERSION 3.20)

project(chess-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/dest
)

file(GLOB sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(chess ${sources})

find_package(Curses REQUIRED)

target_include_directories(chess PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/extern
    ${CURSES_INCLUDE_DIR}
)

target_link_libraries(chess PRIVATE
    ${CURSES_LIBRARIES}
)
