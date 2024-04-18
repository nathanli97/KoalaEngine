include(FetchContent)


function(find_exe EXE PATH)
    set(EXE_NAME ${EXE})
    if (WIN32)
        set(EXE_NAME "${EXE}.exe")
    endif ()

    if (EXISTS "${PATH}/${EXE_NAME}")
        set(FOUND_EXECUTABLE_PATH "${PATH}/${EXE_NAME}" PARENT_SCOPE)
    endif ()
endfunction()

function(check_git)
    message(STATUS "Checking Git...")
    find_package(Git QUIET)
    if (NOT GIT_FOUND)
        message(FATAL_ERROR "Git is found in your system PATH.")
    endif ()
endfunction()

function(check_python)
    find_package(Python REQUIRED Development)
endfunction()

function(discover_dependencies)
    execute_process(
            COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/process_dependencies.py ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies.txt
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE OOOOOO_RESULT
    )
endfunction()

function(process_dependencies)
endfunction()