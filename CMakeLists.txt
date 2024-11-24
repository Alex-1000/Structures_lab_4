cmake_minimum_required(VERSION 3.20.0)
project(
  Structures_lab_3
  VERSION 0.1.0
  LANGUAGES C)

set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(main main.c)
target_link_libraries(main ${CURSES_LIBRARIES})
