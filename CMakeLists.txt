cmake_minimum_required(VERSION 3.18)
project(Wordle VERSION 1.0.0 LANGUAGES C)

file(DOWNLOAD "http://www.gwicks.net/textlists/engmix.zip" ${CMAKE_BINARY_DIR}/engmix.zip)
file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/engmix.zip DESTINATION ${CMAKE_BINARY_DIR})

set(WORDLE_SOURCES "dictionary.c" "getopt.c" "main.c" "wordle_board.c")

add_executable(Wordle ${WORDLE_SOURCES})
