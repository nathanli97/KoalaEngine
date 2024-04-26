cmake_minimum_required(VERSION 3.25)
project(Koala)

option(BUILD_TESTS "Build Tests" ON)

find_package(Vulkan REQUIRED)
find_package(Python REQUIRED Development)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")



add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/eigen3)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vma)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/catch2)

set_target_properties(glfw PROPERTIES FOLDER "ThirdParty")
set_target_properties(fmt PROPERTIES FOLDER "ThirdParty")
set_target_properties(fmt-header-only PROPERTIES FOLDER "ThirdParty")
set_target_properties(spdlog PROPERTIES FOLDER "ThirdParty")
set_target_properties(spdlog_header_only PROPERTIES FOLDER "ThirdParty")
set_target_properties(Catch2 PROPERTIES FOLDER "ThirdParty")
set_target_properties(Catch2WithMain PROPERTIES FOLDER "ThirdParty")



set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Editor)

if (BUILD_TESTS)
    add_subdirectory(Source/Tests)
endif ()