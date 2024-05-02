include(SourceFiles.gen.cmake)
add_executable(KoalaEditor ${MODULE_SOURCE_FILES})
target_compile_features(KoalaEditor PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(KoalaEngine PRIVATE /MP)
endif ()

target_link_libraries(KoalaEditor PRIVATE KoalaEngine)
set_target_properties(KoalaEditor PROPERTIES FOLDER "Engine")
