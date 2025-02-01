cmake_minimum_required(VERSION 3.30)
project(macecraft)

set(CMAKE_CXX_STANDARD 20)

option(RELEASE "Make this a production build" OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw)
add_subdirectory(libs/glad)
add_subdirectory(libs/stb_image)
add_subdirectory(libs/glm)
add_subdirectory(libs/imgui-docking)
add_subdirectory(libs/embed)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(${CMAKE_PROJECT_NAME} ${MY_SOURCES})

if (RELEASE)
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./res/")
else ()
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
endif ()

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

b_embed(${CMAKE_PROJECT_NAME} embed/shaders/default.vert)
b_embed(${CMAKE_PROJECT_NAME} embed/shaders/default.frag)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw glad stb_image glm imgui)


