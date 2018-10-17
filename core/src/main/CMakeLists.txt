cmake_minimum_required(VERSION 3.0.2)
set(project_name vgram_builder)
project(${project_name})

set(CMAKE_CXX_STANDARD 11)
option(WITH_PYTHON "Build with python interface" ON)

set(SOURCE_LIB
        cpp/int_dict.h
        cpp/int_dict.cc
        cpp/int_dict_p.h
        cpp/int_dict_p.cc

        cpp/stat_dict.h
        cpp/stat_dict_p.h
        cpp/stat_dict_p.cc

        cpp/vector_hash.h
        cpp/fast_random.h
        cpp/fast_random.cc

        cpp/int_vgram_builder.h
        cpp/int_vgram_builder_p.h
        cpp/int_vgram_builder_p.cc

        cpp/seq_coder.h
        cpp/seq_coder.cc
#        cpp/int_int_vgram_builder_p.h
#        cpp/int_int_vgram_builder_p.cc

        cpp/list_dict.h
        cpp/list_dict.cc
        cpp/vgram_builder.h
        cpp/vgram_builder.cc
        )

add_library(${project_name} STATIC ${SOURCE_LIB})
