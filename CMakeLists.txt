cmake_minimum_required(VERSION 3.27)
project(koala)

set(CMAKE_CXX_STANDARD 17)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Python REQUIRED Development)


file(GLOB_RECURSE SOURCE_H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.h)
file(GLOB_RECURSE SOURCE_INL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.inl)
file(GLOB_RECURSE SOURCE_C_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.c)
file(GLOB_RECURSE SOURCE_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Engine/Include)

add_executable(koala main.cpp ${SOURCE_H_FILES} ${SOURCE_INL_FILES} ${SOURCE_C_FILES} ${SOURCE_CPP_FILES})
target_include_directories(koala
        PUBLIC ${Vulkan_INCLUDE_DIRS}
)

if (MINGW)
    target_link_libraries(koala PRIVATE ${Vulkan_LIBRARIES} Eigen3::Eigen glfw Python::Python fmt::fmt spdlog::spdlog_header_only)
else ()
    target_link_libraries(koala PRIVATE ${Vulkan_LIBRARIES} Eigen3::Eigen glfw Python::Python fmt::fmt spdlog::spdlog)
endif ()