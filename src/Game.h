#pragma once

#include <memory>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "FrustumCulling/Frustum.h"
#include "Camera.h"
#include "Layer.h"
#include "Renderer.h"
#include "TextureAtlas.h"
#include "WorldLayer.h"

namespace Macecraft
{

class Game 
{
public:
    int width = 1600, height = 900;

    Game();
    ~Game();

    void Run();

    template <class T> T* BindLayer()
    {
        static_assert(std::is_base_of_v<Layer, T> && "T must be a subclass of Layer");

        return static_cast<T*>(m_Layers.emplace_back(new T(*this)).get());
    }

    TextureAtlas* GetAtlas() const;
    Camera* GetCamera() const;
    Shader* GetShader() const;
    Frustum* GetFrustum() const;

private:
    std::vector<std::unique_ptr<Layer>> m_Layers;
    
    GLFWwindow* m_Window = nullptr;
    const char* m_WindowTitle = "Macecraft";

    Camera m_Camera{width, height};
    std::unique_ptr<Shader> m_DefaultShader;
    // std::unique_ptr<WorldLayer> m_World;
    std::unique_ptr<TextureAtlas> m_DefaultAtlas;
    Frustum m_Frustum;

    int m_FPS = 0;
    float m_FrameTime = 0;

    void InitGLFW();
    void InitImGui();
    void InitOpenGL();
    
    void LoadLayers() const;
    void UnloadLayers() const;
    void UpdateLayers(float deltaTime) const;
    void RenderLayers(float deltaTime) const;
    void ImGuiRenderLayers(float deltaTime) const;
    
    void Update(float deltaTime);
    void UpdateOncePerSecond();
    void UpdateImGui(float deltaTime);
};

} // Macecraft
