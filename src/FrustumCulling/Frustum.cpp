#include "Frustum.h"

#include <iostream>

namespace Macecraft
{

void Frustum::UpdateFromCamera(const Camera& camera)
{
    float aspect = camera.getAspectRatio();
    const float halfVSide = camera.farPlane * tanf(camera.fov * 0.5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar3d = camera.farPlane * camera.orientation;

    const glm::vec2 frontMultFar = glm::normalize(glm::vec2(frontMultFar3d.x, frontMultFar3d.z));

    nearPlane = {frontMultFar, glm::dot(glm::vec2(camera.position.x, camera.position.z), frontMultFar) + camera.nearPlane};

    auto rightPlaneNormal = glm::rotate(frontMultFar, glm::radians(90 - camera.fov));
    rightPlane = {rightPlaneNormal,
        glm::dot(glm::vec2(camera.position.x, camera.position.z), rightPlaneNormal)};

    auto leftPlaneNormal = glm::rotate(frontMultFar, glm::radians(-90 + camera.fov));
    leftPlane = {leftPlaneNormal,
        glm::dot(glm::vec2(camera.position.x, camera.position.z), leftPlaneNormal)};

    // std::cout << nearPlane.normal.x << ' ' << nearPlane.normal.y << ' ' << nearPlane.distance << std::endl;
}


} // Macecraft