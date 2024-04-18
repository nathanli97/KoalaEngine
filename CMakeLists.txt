cmake_minimum_required(VERSION 3.25)
project(Koala)
include(CMake/ProcessDependencies.cmake)

check_git()
check_vcpkg()
check_python()


add_executable(koala main.cpp)

set(CMAKE_CXX_STANDARD 20)

