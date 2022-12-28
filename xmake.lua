add_rules("mode.release", "mode.debug")

add_requires("imgui", {configs = {glfw_opengl3 = true}})

target("chip8")
    set_kind("binary")
    add_files("main.cpp")
    add_packages("imgui")
    add_links("pthread")
    add_links("dl")