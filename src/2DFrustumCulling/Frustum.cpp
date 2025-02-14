#include "Frustum.h"

#include <iostream>

namespace Macecraft
{

void Frustum::updateFromCamera(const Camera& camera)
{
    float aspect = camera.getAspectRatio();
    const float halfVSide = camera.farPlane * tanf(camera.fov * 0.5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar3d = camera.farPlane * camera.orientation;

    const glm::vec2 frontMultFar = glm::normalize(glm::vec2(frontMultFar3d.x, frontMultFar3d.z));

    nearPlane = {frontMultFar, glm::dot(glm::vec2(camera.position.x, camera.position.z), frontMultFar) + camera.nearPlane};

    // std::cout << nearPlane.normal.x << ' ' << nearPlane.normal.y << ' ' << nearPlane.distance << std::endl;
}


} // Macecraft