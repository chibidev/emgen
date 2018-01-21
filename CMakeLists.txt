include(ExternalProject)

cmake_minimum_required (VERSION 3.3)

set(CPPAST_BUILD_TEST OFF)
set(CPPAST_BUILD_EXAMPLE OFF)
set(CPPAST_BUILD_TOOL OFF)

add_subdirectory(dependencies/cppast)

project (emgen)

set(CXX_FLAGS "-Wall -Wextra -Weffc++")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS}")

add_executable(emgen src/main.cpp)
target_include_directories(emgen PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(emgen PRIVATE -std=c++14)
target_link_libraries(emgen PRIVATE cppast cxxopts)

