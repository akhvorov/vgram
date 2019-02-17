cmake_minimum_required(VERSION 3.0.2)
set(project_name vgram_builder_core)
project(${project_name})

set(CMAKE_CXX_STANDARD 14)
option(WITH_PYTHON "Build with python interface" ON)


# gtest
#include(src/test/googletest.cmake)
#fetch_googletest(
#        ${PROJECT_SOURCE_DIR}
#        ${PROJECT_BINARY_DIR}/googletest
#)
#enable_testing()

# Google Sanitizer
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O1")

#set(SOURCE_LIB
#        src/main/cpp/int_dict.h
#        src/main/cpp/int_dict.cc
#        src/main/cpp/int_dict_p.h
#        src/main/cpp/int_dict_p.cc
#
#        src/main/cpp/stat_dict.h
#        src/main/cpp/stat_dict.cc
#
#        src/main/cpp/vector_hash.h
#        src/test/cpp/common.h
#        src/test/cpp/common.cc
#        src/main/cpp/fast_random.h
#        src/main/cpp/fast_random.cc
#
#        src/main/cpp/int_vgram_builder_p.h
#        src/main/cpp/int_vgram_builder_p.cc
#
#        src/main/cpp/list_dict.cc
#        src/main/cpp/vgram_builder.cc
#        )
#
#set(TEST_LIB
#        src/test/cpp/int_dict_test.cc
#        src/test/cpp/stat_dict_test.cc
#        src/test/cpp/int_vgram_builder_test.cpp
#        )
#
#add_library(${project_name} STATIC ${SOURCE_LIB})


# tests
#add_executable(${project_name} ${SOURCE_LIB} ${TEST_LIB})

#target_link_libraries(${project_name} gtest gtest_main)

include_directories(src/main)
add_subdirectory(src/main)

#include_directories(src/test)
#add_subdirectory(src/test)
