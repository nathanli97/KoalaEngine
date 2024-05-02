include(SourceFiles.gen.cmake)

add_compile_definitions(INCLUDE_RHI_VULKAN=1)
add_compile_definitions(RHI_ENABLE_GPU_DEBUG=1)
# the following two definitions is work on if RHI_ENABLE_GPU_DEBUG is enabled
add_compile_definitions(RHI_ENABLE_GPU_MARKER=1)
add_compile_definitions(RHI_ENABLE_VALIDATION=1)

set(COMMON_LIBRARIES Eigen3::Eigen glfw Python::Python fmt::fmt GPUOpen::VulkanMemoryAllocator)

add_library(KoalaEngine STATIC ${MODULE_SOURCE_FILES} ${MODULE_INCLUDE_FILES})

set_target_properties(KoalaEngine PROPERTIES FOLDER "Engine")

target_include_directories(KoalaEngine
        PUBLIC "${MODULE_INCLUDE_PATH}"
        PUBLIC ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(KoalaEngine PRIVATE ${Vulkan_LIBRARIES} ${COMMON_LIBRARIES})

if (MINGW)
    target_link_libraries(KoalaEngine PUBLIC spdlog::spdlog_header_only)
else ()
    target_link_libraries(KoalaEngine PUBLIC spdlog::spdlog)
endif ()


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