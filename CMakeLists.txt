cmake_minimum_required(VERSION 3.25)
project(Koala)

include(ThirdParty.cmake)
option(BUILD_TESTS "Build Tests" OFF)

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/BuildEnv.gen.cmake)
    message(STATUS "Using cached environment from GenerateProjectFiles")
    include(${CMAKE_CURRENT_SOURCE_DIR}/BuildEnv.gen.cmake)
    set_koala_build_env()
endif ()

find_package(Python REQUIRED Development)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")


option(VulkanRHI "Build VulkanRHI for rendering" ON)
set(DefaultRHI "VulkanRHI" CACHE STRING "What RHI will be used to rendering by default")

if (${VulkanRHI})
    find_package(Vulkan REQUIRED)
endif ()

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Editor)

if (BUILD_TESTS)
    add_subdirectory(Source/Tests)
endif ()