cmake_minimum_required(VERSION 3.18)
project(Wordle VERSION 1.0.0 LANGUAGES C CXX)

file(DOWNLOAD "http://www.gwicks.net/textlists/engmix.zip" ${CMAKE_BINARY_DIR}/engmix.zip)
file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/engmix.zip DESTINATION ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Wordle/)
add_subdirectory(Wordle-CPP-Console/)
