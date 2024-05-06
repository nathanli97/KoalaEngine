cmake_minimum_required(VERSION 3.25)
project(Koala)

include(ThirdParty.cmake)
option(BUILD_TESTS "Build Tests" OFF)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/BuildEnv.gen.cmake)
    message(STATUS "Using cached environment from GenerateProjectFiles")
    include(${CMAKE_CURRENT_SOURCE_DIR}/BuildEnv.gen.cmake)
    set_koala_build_env()
endif ()

find_package(Python REQUIRED Development)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")

add_common_libs()

option(VulkanRHI "Build VulkanRHI for rendering" ON)
set(DefaultRHI "VulkanRHI" CACHE STRING "What RHI will be used to rendering by default")

if (${VulkanRHI})
    find_package(Vulkan REQUIRED)
    add_vulkan_libs()
endif ()

if (BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/catch2)
    set_target_properties(Catch2 PROPERTIES FOLDER "ThirdParty")
    set_target_properties(Catch2WithMain PROPERTIES FOLDER "ThirdParty")
endif()



set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Editor)

if (BUILD_TESTS)
    add_subdirectory(Source/Tests)
endif ()