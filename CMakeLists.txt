cmake_minimum_required(VERSION 3.0.2)
set(project_name dict_expansion)
#set(BOOST_ROOT /usr/local/Cellar/boost/1.67.0_1/)
project(${project_name})

set(CMAKE_CXX_STANDARD 11)
option(WITH_PYTHON "Build with python interface" ON)

if (WITH_PYTHON)
    add_definitions(-DWITH_PYTHON)

#    find_package(Boost 1.60 REQUIRED COMPONENTS python)
    find_package(Boost 1.60 REQUIRED)
    find_package(PythonLibs REQUIRED)

    include_directories(PUBLIC src/main/cpp)
    include_directories(PUBLIC ${PYTHON_INCLUDE_PATH} ${Boost_INCLUDE_DIRS})
endif (WITH_PYTHON)

add_library(${project_name} SHARED
        src/main/cpp/list_dictionary.cc
        src/main/cpp/list_dictionary.h
        src/main/cpp/int_dictionary.h
        src/main/cpp/vgram_builder.cpp
        src/main/cpp/vgram_builder.h
        src/main/cpp/int_dictionary.cpp
        src/main/cpp/int_dictionary_p.h
        src/main/cpp/int_vgram_builder.h
        src/main/cpp/int_vgram_builder_p.h
        src/main/cpp/int_vgram_builder.cpp)
set_target_properties(${project_name} PROPERTIES PREFIX "")

#
#if (WITH_PYTHON)
#    target_link_libraries(${project_name} ${Boost_LIBRARIES})
#    target_link_libraries(ymaprica ${PYTHON_LIBRARIES})
#    target_link_libraries(metric_test ${PYTHON_LIBRARIES})
#endif (WITH_PYTHON)

