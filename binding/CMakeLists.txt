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

# shared library
#add_library(${project_name} SHARED
#        src/main/cpp/list_dict.h
#        src/main/cpp/int_dict.h
#        src/main/cpp/vgram_builder.h
#        src/main/cpp/int_dict.cc
#        src/main/cpp/int_dict_p.h
#        src/main/cpp/int_vgram_builder.h
#        src/main/cpp/int_vgram_builder_p.h
#        src/main/cpp/int_vgram_builder.cc
#        #src/main/cpp/list_dict.cc
#        #src/main/cpp/vgram_builder.cpp
#        #library.cpp # my
#        #src/test/cpp/int_dict_p_test.cpp
#        )

set(SOURCE_LIB src/main/cpp/main.cc)

pybind11_add_module(${project_name} SHARED ${SOURCE_LIB})
#target_link_libraries(${project_name} PRIVATE core)
#target_link_libraries(${project_name} vgram_builder)