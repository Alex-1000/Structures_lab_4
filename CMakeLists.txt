cmake_minimum_required(VERSION 3.20.0)
project(
  Structures_lab_3
  VERSION 0.1.0
  LANGUAGES C)

set(EXECUTABLE_NAME Structures_lab_3)

add_executable(${EXECUTABLE_NAME} main.c)
configure_file(test_runner.py test_runner.py COPYONLY)

include(CTest)
enable_testing()

# set(RUNNER test_runner.py) set(TEST_CASES "hello123 world"; "+"; "-"; "0";
# "9"; "a+b+c"; "-a-b-c"; "123abc"; "abc123"; "+-abc";
# "++--abc";)
#
# foreach(case ${TEST_CASES}) add_test(NAME "Test ${case}" COMMAND python3
# test_runner.py
# "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME} ${case}")
# endforeach(case TEST_CASES)
