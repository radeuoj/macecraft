#pragma once

#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "FrustumCulling/Frustum.h"
#include "Camera.h"
#include "Renderer.h"
#include "TextureAtlas.h"
#include "World.h"

namespace Macecraft
{

class Game 
{
public:
    int width = 1600, height = 900;

    Game();
    ~Game();

    void Run();
    Camera testCamera{width, height, 45, 0.1, 1000};

private:
    GLFWwindow* m_Window = nullptr;
    const char* m_WindowTitle = "Macecraft";

    Camera m_Camera{width, height, 45.0f, 0.1f, 1000.0f};
    std::unique_ptr<Shader> m_DefaultShader;
    std::unique_ptr<World> m_World;
    std::unique_ptr<TextureAtlas> m_DefaultAtlas;
    Frustum m_Frustum;

    int m_FPS = 0;
    float m_FrameTime = 0;

    void InitGLFW();
    void InitImGui();
    void InitOpenGL();

    void Update(float deltaTime);
    void UpdateOncePerSecond();
    void UpdateImGui(float deltaTime);
};

} // Macecraft
