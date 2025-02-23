workspace "Macecraft"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    startproject "Macecraft"

    flags { "MultiProcessorCompile" }

    cppdialect "C++20"

    filter "configurations:Debug"
        defines { "JET_DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "JET_RELEASE" }
        optimize "Speed"
        linktimeoptimization "On"
        runtime "Release"

    filter "configurations:Dist"
        defines { "JET_DIST" }
        optimize "Speed"
        linktimeoptimization "On"
        runtime "Release"
        
    filter ""
        
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
    
    filter { "configurations:Dist", "system:windows" }
        kind "WindowedApp"
        entrypoint "mainCRTStartup"
        
    filter ""
        