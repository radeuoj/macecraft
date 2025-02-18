#pragma once
#undef min
#undef max
#define NOMINMAX
#include <glm/glm.hpp>

#include "Layer.h"
#include "Renderer.h"

namespace Macecraft {

class PlayerLayer: public Layer
{
public:
    using Layer::Layer;

    void OnLoad() override;
    void OnUpdate(float deltaTime) override;
    void OnRender(float deltaTime) override;
    void OnImGuiRender(float deltaTime) override;

private:
    inline static float NORMAL_SPEED = 10.0f;
    inline static float SPRINT_SPEED = 100.0f;
    inline static float SENSITIVITY = 100.0f;
    
    glm::vec3 m_Position = glm::vec3(0.0f, 15.0f, 0.0f);
    glm::vec3 m_Velocity = glm::vec3(0.0f);
    glm::vec3 m_Orientation = glm::vec3(0.0f, 0.0f, -1.0f);

    std::unique_ptr<Shader> m_UIShader;
    Renderer<UIVertexData> m_StaticUIRenderer;
    // Renderer m_PointedBlockOutlineRenderer;

    void HandleInputs(float deltaTime);
    void HandleBlockLookingAt();
};

}


