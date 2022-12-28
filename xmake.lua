add_rules("mode.debug", "mode.release")

add_requires("imgui", {configs = {glfw_opengl3 = true}})
add_requires("libsdl2")

target("chip8")
    set_kind("binary")
    add_files("src/*.cpp", "main.cpp")
    add_packages("libsdl2")
    add_packages("imgui")