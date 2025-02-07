#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

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
