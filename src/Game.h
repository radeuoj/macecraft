#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Renderer.h"
#include "World.h"

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

    Camera m_Camera{width, height};
    Renderer m_GlobalRenderer;
    Shader m_DefaultShader;
    World m_World;

    int m_FPS = 0;

    // TODO: remove
    Texture dirtTex;

    void initGLFW();
    void initImGui();
    void initOpenGL();

    void update(float deltaTime);
    void updateImGui(float deltaTime);
};

} // Macecraft
