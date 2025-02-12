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
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
        glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        float speed = 1.0f;
        float sensitivity = 100.0f;

        Camera(int &width, int &height);

        void matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
        void inputs(GLFWwindow* window, float deltaTime);

    private:
        int &m_Width, &m_Height;

        bool m_FirstClick = true;
    };

}
