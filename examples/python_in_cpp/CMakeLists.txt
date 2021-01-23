cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

project(python_in_cpp)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify bin path
set(EXECUTABLE_OUTPUT_PATH bin/)

# Link with the "Threads library (required to link with AFF3CT after)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Link with AFF3CT
set (AFF3CT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(AFF3CT CONFIG 2.3.2 REQUIRED)

add_subdirectory(lib/pybind11)

add_executable            (python_in_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
                                         ${CMAKE_CURRENT_SOURCE_DIR}/src/Module/Py_Module/Py_Module.cpp)
target_include_directories(python_in_cpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(python_in_cpp PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
target_link_libraries     (python_in_cpp PRIVATE aff3ct::aff3ct-static-lib)
target_link_libraries     (python_in_cpp PRIVATE pybind11::embed)
