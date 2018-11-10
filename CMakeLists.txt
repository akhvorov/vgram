cmake_minimum_required(VERSION 3.0.2)
#set(project_name vgram)
set(project_name dict_expansion)
project(${project_name})

set(CMAKE_CXX_STANDARD 11)
option(WITH_PYTHON "Build with python interface" ON)

add_subdirectory(core)
add_subdirectory(binding)
