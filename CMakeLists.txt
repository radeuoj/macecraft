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

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${MY_SOURCES})

if (RELEASE)
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./res/")

    if (WIN32)
        if (MSVC)
            set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console
        else ()
            set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "-mwindows") #no console
        endif ()
    endif ()
else ()
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
endif ()

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/res/ ${CMAKE_CURRENT_BINARY_DIR}/res COPYONLY)
#add_custom_command(
#        TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        ${CMAKE_SOURCE_DIR}/res/*
#        ${CMAKE_CURRENT_BINARY_DIR}/res)
#add_custom_command(TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/res)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw glad stb_image glm imgui)


