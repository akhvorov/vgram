cmake_minimum_required(VERSION 3.0.2)
set(project_name vgram_builder)
project(${project_name})

set(CMAKE_CXX_STANDARD 14)
option(WITH_PYTHON "Build with python interface" ON)

set(SOURCE_LIB
        cpp/fast_random.cc
        cpp/fast_random.h

        cpp/int_dict.cc
        cpp/int_dict.h
        cpp/int_dict_p.cc
        cpp/int_dict_p.h

        cpp/int_vgram_builder.h
        cpp/int_vgram_builder_p.cc
        cpp/int_vgram_builder_p.h

        cpp/list_dict.cc
        cpp/list_dict.h

        ../../../binding/src/main/cpp/seq_coder.cc
        ../../../binding/src/main/cpp/seq_coder.h

        cpp/stat_dict.h
        cpp/stat_dict_p.cc
        cpp/stat_dict_p.h

        cpp/vector_hash.h

        cpp/vgram_builder.h
        cpp/vgram_builder.cc
        )

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(${project_name} STATIC ${SOURCE_LIB})
