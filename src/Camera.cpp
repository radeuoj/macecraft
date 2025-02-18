#include "Camera.h"

namespace Macecraft
{

Camera::Camera(int &width, int &height):
    m_Width(width),
    m_Height(height)
{
}

void Camera::Matrix(const Shader* shader, const char *uniform)
{
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    view = glm::lookAt(position, position + orientation, UP);
    proj = glm::perspective(glm::radians(FOV), (float)m_Width / m_Height, NEAR_PLANE, FAR_PLANE);
    viewproj = proj * view;

    glUniformMatrix4fv(glGetUniformLocation(shader->GetID(), uniform), 1, GL_FALSE, glm::value_ptr(viewproj));
}

float Camera::GetAspectRatio() const
{
    return float(m_Width) / float(m_Height);
}

}



