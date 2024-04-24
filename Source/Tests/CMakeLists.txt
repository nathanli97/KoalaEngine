message(STATUS "Including Engine tests")
include(SourceFiles.gen.cmake)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

foreach (TestFile IN ITEMS ${MODULE_SOURCE_FILES})
    string(REGEX REPLACE "\.cpp" "" FileNameNoExt ${TestFile})
    string(REGEX REPLACE ".*(/|\\\\)" "" FileNameNoExt ${FileNameNoExt})
    add_executable(${FileNameNoExt} ${TestFile})
    target_include_directories(
            ${FileNameNoExt}
            PUBLIC ${gtest_SOURCE_DIR}/include
    )
    target_link_libraries(${FileNameNoExt} KoalaEngine Catch2::Catch2 Catch2::Catch2WithMain)

    set_target_properties(${FileNameNoExt} PROPERTIES FOLDER "EngineTests")

endforeach ()
