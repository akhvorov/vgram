cmake_minimum_required(VERSION 3.0.2)
set(project_name vgram_interfaces)
project(${project_name})

set(CMAKE_CXX_STANDARD 14)
option(WITH_PYTHON "Build with python interface" ON)

set(SOURCE_LIB
        vgram_builders/int_vgram.h
        vgram_builders/int_vgram.cc
        vgram_builders/int_stream_vgram.h
        vgram_builders/int_stream_vgram.cc
        vgram_builders/string_stream_vgram.h
        vgram_builders/string_stream_vgram.cc
        vgram_builders/string_vgram.h
        vgram_builders/string_vgram.cc
        tokenizers/base_tokenizer.h
        tokenizers/base_tokenizer.cc
        tokenizers/char_tokenizer.h
        tokenizers/char_tokenizer.cc
        seq_coder.h
        seq_coder.cc
        )

include_directories(../algorithm)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(${project_name} STATIC ${SOURCE_LIB})
target_link_libraries(${project_name} vgram_builder)
#target_include_directories(${project_name} ../algorithm)
