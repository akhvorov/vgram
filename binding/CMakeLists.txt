#cmake_minimum_required(VERSION 3.0.2)
#set(project_name vgram)
#project(${project_name})
#
#set(CMAKE_CXX_STANDARD 11)
#option(WITH_PYTHON "Build with python interface" ON)
#
## pybind11
#include(pybind11.cmake)
#fetch_pybind11(
#        ${PROJECT_SOURCE_DIR}
#        ${PROJECT_BINARY_DIR}/pybind11
#)
#
#include_directories(../core)
#
#set(SOURCE_LIB
#        src/main/cpp/bind_all.cc
#        src/main/cpp/bind_vgram_builders.cc
#        src/main/cpp/bind_tokenizers.cc
#        src/main/cpp/json.h
#        src/main/cpp/py_vgram_builder.h
#        src/main/cpp/py_vgram_builder.cc
#        src/main/cpp/py_stream_vgram_builder.h
#        src/main/cpp/py_stream_vgram_builder.cc
#        src/main/cpp/base_tokenizer.h
#        src/main/cpp/base_tokenizer.cc
#        src/main/cpp/char_tokenizer.h
#        src/main/cpp/char_tokenizer.cc
#        )
#
#pybind11_add_module(${project_name} ${SOURCE_LIB})
##target_link_libraries(${project_name} PRIVATE ${CMAKE_SOURCE_DIR}/libvgram_builder.a)
#target_link_libraries(${project_name} PUBLIC vgram_builder)

include_directories(../core)
add_subdirectory(src/main)
