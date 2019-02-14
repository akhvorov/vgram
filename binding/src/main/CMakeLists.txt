cmake_minimum_required(VERSION 3.0.2)
set(project_name vgram)
project(${project_name})

set(CMAKE_CXX_STANDARD 11)
option(WITH_PYTHON "Build with python interface" ON)

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
        cpp/json.h
        cpp/py_vgram_builder.h
        cpp/py_vgram_builder.cc
        cpp/py_stream_vgram_builder.h
        cpp/py_stream_vgram_builder.cc
        cpp/tokenizers/base_tokenizer.h
        cpp/tokenizers/base_tokenizer.cc
        cpp/tokenizers/char_tokenizer.h
        cpp/tokenizers/char_tokenizer.cc
        cpp/coders/seq_coder.h
        cpp/coders/seq_coder.cc
        )

pybind11_add_module(${project_name} ${SOURCE_LIB})
target_link_libraries(${project_name} PRIVATE vgram_builder)
