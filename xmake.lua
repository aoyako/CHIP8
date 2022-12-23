add_rules("mode.release", "mode.debug")

target("chip8")
    set_kind("binary")
    add_files("main.cpp")