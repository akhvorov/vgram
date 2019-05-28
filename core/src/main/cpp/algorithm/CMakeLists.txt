cmake_minimum_required(VERSION 3.0.2)
set(project_name vgram_builder)
project(${project_name})

set(CMAKE_CXX_STANDARD 14)
option(WITH_PYTHON "Build with python interface" ON)

set(SOURCE_LIB
        fast_random.cc
        fast_random.h

        int_dict.cc
        int_dict.h
        int_dict_p.cc
        int_dict_p.h

        int_vgram_builder.h
        int_vgram_builder_p.cc
        int_vgram_builder_p.h

        list_dict.cc
        list_dict.h

        stat_dict.h
        stat_dict_p.cc
        stat_dict_p.h

        vector_hash.h

        vgram_builder.h
        vgram_builder.cc
        )

#include_directories(../interfaces)
#add_subdirectory(../interfaces)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(${project_name} STATIC ${SOURCE_LIB})
