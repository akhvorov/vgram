cmake_minimum_required(VERSION 3.0.2)
#set(project_name vgram)
set(project_name dict_expansion)
project(${project_name})

set(CMAKE_CXX_STANDARD 11)
option(WITH_PYTHON "Build with python interface" ON)

include_directories(core/src/main)
add_subdirectory(core/src/main)

include_directories(core/src/test)
add_subdirectory(core/src/test)

include_directories(binding)
add_subdirectory(binding)
