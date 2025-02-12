cmake_minimum_required(VERSION 3.20)
project(macecraft)

set(CMAKE_CXX_STANDARD 20)

# TODO: make this get the build type, debug or release so i dont have to delete the cmake cache every time
option(RELEASE "Make this a production build" OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(B_EMBED_SILENCE_DEVMODE_WARNING ON CACHE BOOL "" FORCE)

add_subdirectory(libs)
add_subdirectory(libs/glfw)
add_subdirectory(libs/stb_image)
add_subdirectory(libs/glm)
add_subdirectory(libs/embed)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(${CMAKE_PROJECT_NAME} ${MY_SOURCES})

if (WIN32 AND RELEASE)
    if (MSVC)
        set_property(TARGET ${CMAKE_PROJECT_NAME} glfw glad stb_image glm imgui PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console
#        set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded")
#        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else ()
        set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "-mwindows") #no console
    endif ()
endif ()

#if (RELEASE)
#    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./res/")
#else ()
#    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
#endif ()

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

#include(libs/cmrc/CMakeRC.cmake)
#
#cmrc_add_resource_library(embed embed/textures/dirt.png)

#execute_process(COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/image_to_raw.py ${CMAKE_CURRENT_SOURCE_DIR}/res/textures/dirt.png ${CMAKE_CURRENT_SOURCE_DIR}/embed/textures/dirt.png.bytes)

file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/embed/textures/dirt.raw ${CMAKE_CURRENT_SOURCE_DIR}/embed/textures/debug.raw)

add_custom_command(
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/image_to_raw.py ${CMAKE_CURRENT_SOURCE_DIR}/res/textures/dirt.png ${CMAKE_CURRENT_SOURCE_DIR}/embed/textures/dirt.raw
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/embed/textures/dirt.raw
)

add_custom_command(
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/image_to_raw.py ${CMAKE_CURRENT_SOURCE_DIR}/res/textures/debug.png ${CMAKE_CURRENT_SOURCE_DIR}/embed/textures/debug.raw
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/embed/textures/debug.raw
)

b_embed(${CMAKE_PROJECT_NAME} embed/textures/dirt.raw)
b_embed(${CMAKE_PROJECT_NAME} embed/textures/debug.raw)
b_embed(${CMAKE_PROJECT_NAME} embed/shaders/default.vert)
b_embed(${CMAKE_PROJECT_NAME} embed/shaders/default.frag)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw glad stb_image glm imgui)


