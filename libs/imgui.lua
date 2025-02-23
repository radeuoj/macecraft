project "imgui"
    kind "StaticLib"
    language "C++"
    staticruntime "off"
    
    location "imgui"

    files {
        "imgui/imgui.cpp",
        "imgui/imgui_demo.cpp",
        "imgui/imgui_draw.cpp",
        "imgui/imgui_tables.cpp",
        "imgui/imgui_widgets.cpp",
        "imgui/backends/imgui_impl_glfw.cpp",
        "imgui/backends/imgui_impl_opengl3.cpp",
        "imgui/**.h",
    }
    
    links {
        "glfw",
        "glad",
    }

    includedirs {
        "imgui",  
        "imgui/backends",
        "glfw/include"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter { "configurations:Release", "configurations:Dist" }
        runtime "Release"
        optimize "on"