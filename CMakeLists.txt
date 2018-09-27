cmake_minimum_required(VERSION 3.0.2)
#set(project_name vgram)
set(project_name dict_expansion)
#set(BOOST_ROOT /usr/local/Cellar/boost/1.67.0_1/)
# set(BOOST_ROOT /usr/local/boost_1_67_0/) # my
project(${project_name})

set(CMAKE_CXX_STANDARD 11)
option(WITH_PYTHON "Build with python interface" ON)

# boost.python
#if (WITH_PYTHON)
#    add_definitions(-DWITH_PYTHON)
#
#    find_package(Boost 1.60 REQUIRED COMPONENTS python)
#    find_package(Boost 1.60 REQUIRED)
#    find_package(PythonLibs REQUIRED)
#
#    include_directories(PUBLIC src/main/cpp)
#    include_directories(PUBLIC ${PYTHON_INCLUDE _PATH} ${Boost_INCLUDE_DIRS})
#endif (WITH_PYTHON)

# gtest
#include(googletest.cmake)
#fetch_googletest(
#        ${PROJECT_SOURCE_DIR}
#        ${PROJECT_BINARY_DIR}/googletest
#)
#
#enable_testing()

# pybind11
#include(pybind11.cmake)
#fetch_pybind11(
#        ${PROJECT_SOURCE_DIR}
#        ${PROJECT_BINARY_DIR}/pybind11
#)

# Google Sanitizer
# http://www.jetbrains.com/help/clion/google-sanitizers.html
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer -g -O1")

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

# for tests
#add_executable(${project_name}
#        core/src/main/cpp/int_dict.h
#        core/src/main/cpp/int_dict.cc
#        core/src/main/cpp/int_dict_p.h
#        core/src/main/cpp/int_dict_p.cc
#        core/src/test/cpp/int_dict_test.cc
#
#        core/src/main/cpp/stat_dict.h
#        core/src/main/cpp/stat_dict.cc
#        core/src/test/cpp/stat_dict_test.cc
#
#        core/src/main/cpp/vector_hash.h
#        core/src/test/cpp/common.h
#        core/src/test/cpp/common.cc
#        core/src/main/cpp/fast_random.h
#        core/src/main/cpp/fast_random.cc
#
#        core/src/main/cpp/int_vgram_builder_p.h
#        core/src/main/cpp/int_vgram_builder_p.cc
#        core/src/test/cpp/int_vgram_builder_test.cpp
#
##        src/main/cpp/main.cc
#        core/src/main/cpp/list_dict.cc
#        core/src/main/cpp/vgram_builder.cc)
#
#pybind11_add_module(vgram #${project_name}
#        core/src/main/cpp/int_dict.h
#        core/src/main/cpp/int_dict.cc
#        core/src/main/cpp/int_dict_p.h
#        core/src/main/cpp/int_dict_p.cc
#
#        core/src/main/cpp/stat_dict.h
#        core/src/main/cpp/stat_dict.cc
#
#        core/src/main/cpp/vector_hash.h
#        core/src/test/cpp/common.h
#        core/src/test/cpp/common.cc
#        core/src/main/cpp/fast_random.h
#        core/src/main/cpp/fast_random.cc
#
#        core/src/main/cpp/int_vgram_builder_p.h
#        core/src/main/cpp/int_vgram_builder_p.cc
#
#        core/src/main/cpp/list_dict.cc
#        core/src/main/cpp/vgram_builder.cc
#        core/src/main/cpp/main.cc)



# add_library(vgram SHARED library.cpp) # my
# set_target_properties(${project_name} PROPERTIES PREFIX "")

# boost.python
#if (WITH_PYTHON)
#    target_link_libraries(${project_name} ${Boost_LIBRARIES})
#endif (WITH_PYTHON)

#target_link_libraries(${project_name} gtest gtest_main)

include_directories(core)
add_subdirectory(core)

include_directories(binding)
add_subdirectory(binding)

