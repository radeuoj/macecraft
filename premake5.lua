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
    targetdir "bin/%{cfg.buildcfg}"

    files { "src/**.h", "src/**.cpp" }