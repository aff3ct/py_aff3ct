cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

project (py_aff3ct_test)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify bin path
set (EXECUTABLE_OUTPUT_PATH bin/)

# Link with the "Threads library (required to link with AFF3CT after)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Link with AFF3CT
set (AFF3CT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(AFF3CT CONFIG 2.3.2 REQUIRED)

add_subdirectory(lib/pybind11)

file(GLOB_RECURSE SRC_FILES_CO ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
add_library (py_aff3ct_obj STATIC ${SRC_FILES_CO})
target_link_libraries     (py_aff3ct_obj PRIVATE aff3ct::aff3ct-static-lib)
target_link_libraries     (py_aff3ct_obj PRIVATE pybind11::embed)
target_include_directories(py_aff3ct_obj PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

pybind11_add_module(py_aff3ct ${CMAKE_CURRENT_SOURCE_DIR}/src/py_aff3ct.cpp ${SRC_FILES_CO})
target_link_libraries     (py_aff3ct PRIVATE aff3ct::aff3ct-static-lib)
target_include_directories(py_aff3ct PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable            (py_aff3ct_test ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp)
target_link_libraries     (py_aff3ct_test PRIVATE aff3ct::aff3ct-static-lib)
target_compile_definitions(py_aff3ct_test PRIVATE NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)
target_link_libraries     (py_aff3ct_test PRIVATE pybind11::embed)
target_link_libraries     (py_aff3ct_test PRIVATE py_aff3ct_obj)
target_include_directories(py_aff3ct_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
