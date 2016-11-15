cmake_minimum_required(VERSION 3.5)
project("SYSU Agenda" CXX)
add_compile_options(-std=c++14)

include_directories(include)
link_directories(build)

aux_source_directory(src SOURCES)
add_library(agenda SHARED ${SOURCES})

add_executable(Agenda "${SOURCES}")
