add_executable(KoalaEditor main.cpp)
target_compile_features(KoalaEditor PRIVATE cxx_std_17)

target_link_libraries(KoalaEditor PRIVATE KoalaEngine)
set_target_properties(KoalaEditor PROPERTIES FOLDER "Engine")
