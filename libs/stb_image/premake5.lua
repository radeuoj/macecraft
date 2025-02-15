project "stb_image"
    kind "StaticLib"
    language "C++"
    
    files {
        "src/stb_image.cpp",
        "include/stb_image/stb_image.h"
    }

    includedirs {
        "include"
    }