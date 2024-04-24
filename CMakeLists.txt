cmake_minimum_required(VERSION 3.25)
project(Koala)
include(CMake/ProcessDependencies.cmake)

process_dependencies()

find_package(Vulkan REQUIRED)
find_package(Python REQUIRED Development)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")


if (${GIT_SUBMODULE_RESOLVED})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glfw)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/eigen3)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fmt)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/spdlog)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vma)

    set_target_properties(glfw PROPERTIES FOLDER "ThirdParty")
    set_target_properties(fmt PROPERTIES FOLDER "ThirdParty")
    set_target_properties(fmt-header-only PROPERTIES FOLDER "ThirdParty")
    set_target_properties(spdlog PROPERTIES FOLDER "ThirdParty")
    set_target_properties(spdlog_header_only PROPERTIES FOLDER "ThirdParty")
endif ()


set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Editor)