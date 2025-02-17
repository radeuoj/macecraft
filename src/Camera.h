#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Shader.h"

namespace Macecraft
{

class Camera {
public:
    inline static float NORMAL_SPEED = 11.0f;
    inline static float SPRINT_SPEED = 100.0f;
    
    glm::vec3 position = glm::vec3(0.0f, 15.0f, 0.0f);
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewproj = glm::mat4(1.0f);

    float nearPlane = 0.1f, farPlane = 1000.0f, fov = 60.0f;

    float speed = NORMAL_SPEED;
    float sensitivity = 100.0f;

    Camera(int &width, int &height);

    void Matrix(const Shader* shader, const char* uniform);
    void Inputs(GLFWwindow* window, float deltaTime);

    float GetAspectRatio() const;

private:
    int &m_Width, &m_Height;

    bool m_FirstClick = true;
};

}
