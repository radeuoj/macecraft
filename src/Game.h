#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "FrustumCulling/Frustum.h"
#include "Camera.h"
#include "Layer.h"
#include "Renderer.h"
#include "TextureAtlas.h"

#include <typeinfo>

#include "TextureArray.h"

namespace Macecraft
{

class Game 
{
public:
    int width = 1600, height = 900;

    Game();
    ~Game();

    void Run();

    // template <class T> void BindLayer(Layer* layer)
    void BindLayer(Layer* layer)
    {
        // static_assert(std::is_base_of_v<Layer, T> && "T must be a subclass of Layer");

        layer->__Internal_SetGame(this);
        m_Layers.push_back(layer);

        // return dynamic_cast<T*>(m_Layers.emplace_back(new T(*this)).get());
    }

    // const TextureAtlas* GetAtlas() const;
    const TextureArray* GetTextureArray() const;
    Camera* GetCamera() const;
    const Shader* GetShader() const;
    const Frustum* GetFrustum() const;
    GLFWwindow* GetWindow() const;

    template <class T> T* GetLayer() const
    {
        static_assert(std::is_base_of_v<Layer, T> && "T must be a subclass of Layer");

        for (Layer* layer : m_Layers)
        {
            // std::cout << typeid(*layer.get()).name() << " : " << typeid(T).name() << '\n';
            if (typeid(*layer) == typeid(T))
            {
                return dynamic_cast<T*>(layer);
            }
        }

        // TODO: use custom asserts
        assert(false && "Layer not found");

        return nullptr;
    }

private:
    std::vector<Layer*> m_Layers;
    
    GLFWwindow* m_Window = nullptr;
    const char* m_WindowTitle = "Macecraft";

    Camera m_Camera{width, height};
    std::unique_ptr<Shader> m_DefaultShader;
    // std::unique_ptr<TextureAtlas> m_DefaultAtlas;
    std::unique_ptr<TextureArray> m_DefaultTextureArray;
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
    void UIRenderLayers(float deltaTime) const;
    void ImGuiRenderLayers(float deltaTime) const;
    
    void Update(float deltaTime);
    void UpdateImGui(float deltaTime);
};

} // Macecraft
