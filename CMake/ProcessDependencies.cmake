function(process_dependencies)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
    find_package(Git QUIET)
    if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        execute_process(
                COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT
        )
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()

        message(STATUS "Required 3rdparty packages resolved via git-submodule")
        set(GIT_SUBMODULE_RESOLVED 1 PARENT_SCOPE)
    else ()
        # By the default, the following packages should be resolved by git submodule
        message(WARNING "Git not found or current directory is not a git repo. Will lookup required packages manually.")
        find_package(glfw3 CONFIG REQUIRED)
        find_package(Eigen3 CONFIG REQUIRED)
        find_package(fmt CONFIG REQUIRED)
        find_package(spdlog CONFIG REQUIRED)
        find_package(VulkanMemoryAllocator CONFIG REQUIRED)
    endif ()
endfunction()