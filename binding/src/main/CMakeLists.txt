cmake_minimum_required(VERSION 3.0.2)
set(project_name vgram)
project(${project_name})

set(CMAKE_CXX_STANDARD 14)
option(WITH_PYTHON "Build with python interface" ON)

set(PYBIND11_CPP_STANDARD -std=c++14)

# pybind11
include(pybind11.cmake)
fetch_pybind11(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}/pybind11
)

set(SOURCE_LIB
        cpp/bind_all.cc
        cpp/bind_vgram_builders.cc
        cpp/bind_tokenizers.cc
        cpp/py_int_vgram_builder.h
        cpp/py_vgram_builder.h
        cpp/base_tokenizer.h
        cpp/py_tokenizers.h
#        cpp/test.cc
        )

pybind11_add_module(${project_name} ${SOURCE_LIB})
target_link_libraries(${project_name} PRIVATE vgram_interfaces)

#add_executable(${project_name} ${SOURCE_LIB})
#pybind11_add_module(test_cpp ${SOURCE_LIB})
