#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Chunk.h"
#include "Renderer.h"
#include "VertexData.h"

#include "battery/embed.hpp"

namespace Macecraft
{

class Game 
{
public:
    int width = 1600, height = 900;

    Game();
    ~Game();

    void run();
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* m_Window = nullptr;
    const char* m_WindowTitle = "Macecraft";
    Camera m_Camera;

    void initGLFW();
    void initImGui();
    void initOpenGL();
};

} // Macecraft
