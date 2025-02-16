#include "Camera.h"

namespace Macecraft
{

Camera::Camera(int &width, int &height, float FOVdeg, float _nearPlane, float _farPlane):
    m_Width(width),
    m_Height(height),
    fov(FOVdeg),
    nearPlane(_nearPlane),
    farPlane(_farPlane)
{
}

void Camera::Matrix(const Shader* shader, const char *uniform)
{
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    view = glm::lookAt(position, position + orientation, up);
    proj = glm::perspective(glm::radians(fov), (float)m_Width / m_Height, nearPlane, farPlane);
    viewproj = proj * view * 0.5f;

    glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), uniform), 1, GL_FALSE, glm::value_ptr(viewproj));
}

void Camera::Inputs(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += speed * orientation * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position += speed * -orientation * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position += speed * -glm::normalize(glm::cross(orientation, up)) * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += speed * glm::normalize(glm::cross(orientation, up)) * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        position += speed * up * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        position += speed * -up * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        speed = 100.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        speed = 1.0f;
    }

    // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (m_FirstClick)
        {
            glfwSetCursorPos(window, m_Width / 2, m_Height / 2);
            m_FirstClick = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotX = sensitivity * (float)(mouseY - (float)m_Height / 2) / (float)m_Height;
        float rotY = sensitivity * (float)(mouseX - (float)m_Width / 2) / (float)m_Width;

        glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

        if (!(glm::angle(newOrientation, up) <= glm::radians(5.0f) or glm::angle(newOrientation, -up) <= glm::radians(5.0f)))
        {
            orientation = newOrientation;
        }

        orientation = glm::rotate(orientation, glm::radians(-rotY), up);

        glfwSetCursorPos(window, (float)m_Width / 2, (float)m_Height / 2);
    }
    // else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_FirstClick = true;
    }
}

float Camera::GetAspectRatio() const
{
    return float(m_Width) / float(m_Height);
}

}



