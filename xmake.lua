add_rules("mode.debug", "mode.release")

add_requires("libsdl2")
add_requires("libsdl_mixer")

target("chip8")
    set_kind("binary")
    add_files("src/*.cpp", "main.cpp")
    add_packages("libsdl2")
    add_packages("libsdl_mixer")
    add_cxxflags("-Wall", "-Wextra")