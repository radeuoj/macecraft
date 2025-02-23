#pragma once

#include "glm.h"

#include "Shader.h"

namespace Macecraft
{

class Camera {
public:
    static constexpr glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
    
    glm::vec3 position = glm::vec3(0.0f, 15.0f, 0.0f);
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::mat4 viewproj = glm::mat4(1.0f);

    static constexpr float NEAR_PLANE = 0.1f, FAR_PLANE = 1000.0f, FOV = 60.0f;

    Camera(int &width, int &height);

    void Matrix(const Shader* shader, const char* uniform);

    float GetAspectRatio() const;

private:
    int &m_Width, &m_Height;
};

}
