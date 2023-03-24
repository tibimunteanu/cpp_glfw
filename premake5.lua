workspace "cpp_glfw"
    architecture "x86_64"
    startproject "cpp_glfw"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "cpp_glfw"

