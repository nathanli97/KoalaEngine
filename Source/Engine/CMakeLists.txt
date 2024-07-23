include(SourceFiles.gen.cmake)
include(CMakeDependentOption)
add_compile_definitions(INCLUDE_RHI_VULKAN=1)
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(Python REQUIRED Development)
find_package(volk REQUIRED)

option(RHI_GPU_DEBUG "Enable GPU Debug features" ON)
option(ENABLE_CPU_PROFILE "Enable CPU Markers for profiling (PIX for Windows)" ON)
cmake_dependent_option(RHI_GPU_MARKER "Enable GPU Markers (Symbols for GPU Debugging)" ON "RHI_GPU_DEBUG" OFF)
cmake_dependent_option(RHI_GPU_VALIDATION "Enable GPU Validation" ON "RHI_GPU_DEBUG" OFF)

if (${RHI_GPU_DEBUG})
    message(STATUS "KoalaEngine: Enabling GPU Debug features")
    add_compile_definitions(RHI_ENABLE_GPU_DEBUG=1)
    if (${RHI_GPU_MARKER})
        add_compile_definitions(RHI_ENABLE_GPU_MARKER=1)
        message(STATUS "KoalaEngine: Enabled GPU DEBUG")
    endif ()
    if (${RHI_GPU_VALIDATION})
        add_compile_definitions(RHI_ENABLE_VALIDATION=1)
        message(STATUS "KoalaEngine: Enabled GPU VALIDATION")
    endif ()
endif ()

if (${ENABLE_CPU_PROFILE})
    message(STATUS "KoalaEngine: Enabling CPU Profiling Markers")
    add_compile_definitions(ENABLE_CPU_PROFILE=1)
endif ()

set(COMMON_LIBRARIES glfw GPUOpen::VulkanMemoryAllocator volk::volk)

add_library(KoalaEngine STATIC ${MODULE_SOURCE_FILES} ${MODULE_INCLUDE_FILES})

set_target_properties(KoalaEngine PROPERTIES FOLDER "Engine")

target_include_directories(KoalaEngine
        PUBLIC "${MODULE_INCLUDE_PATH}"
        PUBLIC ${Vulkan_INCLUDE_DIRS}
)

if (${ENABLE_CPU_PROFILE})
    target_link_libraries(KoalaEngine PUBLIC ${CMAKE_SOURCE_DIR}/ThirdParty/Target/lib/WinPixEventRuntime.lib)
endif ()

target_link_libraries(KoalaEngine PRIVATE ${Vulkan_LIBRARIES} ${COMMON_LIBRARIES})

target_link_libraries(KoalaEngine PUBLIC spdlog::spdlog_header_only Eigen3::Eigen Python::Python fmt::fmt-header-only)



target_compile_features(KoalaEngine PUBLIC cxx_std_17)

if (MSVC)
    target_compile_options(KoalaEngine PRIVATE /MP)
endif ()

source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}/Include/
        PREFIX "Include"
        FILES ${MODULE_INCLUDE_FILES}
)

source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source/
        PREFIX "Source"
        FILES ${MODULE_SOURCE_FILES}
)

source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        PREFIX "CMakeFiles"
        FILES CMakeLists.txt
)