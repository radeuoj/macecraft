workspace "Macecraft"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Macecraft"

    flags { "MultiProcessorCompile" }

    cppdialect "C++20"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"
        linktimeoptimization "On"
        
include "libs"

project "Macecraft"
    kind "ConsoleApp"
    language "C++"
    --targetdir "bin/%{cfg.buildcfg}"

    files { "src/**.h", "src/**.cpp" }
    
    links {
        "glad",
        "glfw",
        "imgui",
        "stb_image",
    }
    
    includedirs {
        "libs/glad/include",
        "libs/glfw/include",
        "libs/imgui",
        "libs/imgui/backends",
        "libs/glm",
        "libs/stb_image/include"
    }