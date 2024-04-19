cmake_minimum_required(VERSION 3.25)
project(Koala)
include(CMake/ProcessDependencies.cmake)

process_dependencies()

find_package(Vulkan REQUIRED)
find_package(Python REQUIRED Development)

if (${GIT_SUBMODULE_RESOLVED})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glfw)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/eigen3)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fmt)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/spdlog)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vma)
endif ()


set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Include/Engine/*.h)
file(GLOB_RECURSE SOURCE_H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/*.h)
file(GLOB_RECURSE SOURCE_INL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/*.inl)
file(GLOB_RECURSE SOURCE_C_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/*.c)
file(GLOB_RECURSE SOURCE_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Engine/*.cpp)


add_compile_definitions(INCLUDE_RHI_VULKAN=1)
add_compile_definitions(RHI_ENABLE_VALIDATION=1)

set(COMMON_LIBRARIES Eigen3::Eigen glfw Python::Python fmt::fmt GPUOpen::VulkanMemoryAllocator)
add_executable(koala main.cpp ${SOURCE_H_FILES} ${SOURCE_INL_FILES} ${SOURCE_C_FILES} ${SOURCE_CPP_FILES} ${INCLUDE_FILES})

target_include_directories(koala
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include/Engine"
        PUBLIC ${Vulkan_INCLUDE_DIRS}
)

if (MINGW)
    target_link_libraries(koala PRIVATE ${Vulkan_LIBRARIES} ${COMMON_LIBRARIES} spdlog::spdlog_header_only)
else ()
    target_link_libraries(koala PRIVATE ${Vulkan_LIBRARIES} ${COMMON_LIBRARIES} spdlog::spdlog)
endif ()