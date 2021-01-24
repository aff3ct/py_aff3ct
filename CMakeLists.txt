cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

project (py_aff3ct_test)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify lib path
set(LIBRARY_OUTPUT_PATH lib/)

# Link with the "Threads library (required to link with AFF3CT after)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Link with AFF3CT
set (AFF3CT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(AFF3CT CONFIG 2.3.2 REQUIRED)

add_subdirectory(lib/pybind11)

file(GLOB_RECURSE SRC_FILES_CO ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

# `pybind11_add_module` is a wrapper of the CMake `add_library` function
pybind11_add_module(py_aff3ct-module-lib MODULE ${CMAKE_CURRENT_SOURCE_DIR}/src/py_aff3ct.cpp ${SRC_FILES_CO})
set_target_properties(py_aff3ct-module-lib PROPERTIES
                                           OUTPUT_NAME py_aff3ct
                                           POSITION_INDEPENDENT_CODE ON) # set -fpic
target_link_libraries(py_aff3ct-module-lib PRIVATE aff3ct::aff3ct-static-lib)
target_include_directories(py_aff3ct-module-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
