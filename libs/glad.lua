project "glad"
    kind "StaticLib"
    language "C"
    
    location "glad"
    
    includedirs { "glad/include/" }

    files { "glad/src/glad.c", "glad/include/**" }
    
    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "On"

        defines {
            "_GLAD_X11"
        }

    filter "system:windows"
        systemversion "latest"
        staticruntime "On"

        defines {
            "_GLAD_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }