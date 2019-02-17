cmake_minimum_required(VERSION 3.0.2)
set(project_name vgram_builder_test)
project(${project_name})

set(CMAKE_CXX_STANDARD 14)
option(WITH_PYTHON "Build with python interface" ON)


# gtest
include(googletest.cmake)
fetch_googletest(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}/googletest
)
enable_testing()

# Google Sanitizer
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O1")

set(TEST_LIB
        cpp/common.h
        cpp/common.cc
        cpp/int_dict_test.cc
        cpp/stat_dict_test.cc
        cpp/int_vgram_builder_test.cpp
        )

# tests
add_executable(${project_name} ${TEST_LIB})
#target_link_libraries(${project_name} vgram_builder)
target_link_libraries(${project_name} vgram_builder gtest gtest_main)
