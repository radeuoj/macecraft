#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader.h"

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    bool firstClick = true;

    unsigned int *width, *height;

    float speed = 1.0f;
    float sensitivity = 100.0f;

    Camera(unsigned int* width, unsigned int* height, glm::vec3 position);

    void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
    void Inputs(GLFWwindow* window, float deltaTime);
};
