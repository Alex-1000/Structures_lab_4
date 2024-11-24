cmake_minimum_required(VERSION 3.20.0)
project(
  Structures_lab_3
  VERSION 0.1.0
  LANGUAGES C)

add_executable(linked_list linked_list.c)
add_executable(linked_two_way_list linked_two_way_list.c)

if(UNIX)
  set(CURSES_NEED_WIDE TRUE)
  find_package(Curses REQUIRED)
  include_directories(${CURSES_INCLUDE_DIR})

  target_link_libraries(linked_list ${CURSES_LIBRARIES})
  target_link_libraries(linked_two_way_list ${CURSES_LIBRARIES})
else(UNIX)
  include_directories("D:\\msys2\\mingw64\\include")
endif(UNIX)
