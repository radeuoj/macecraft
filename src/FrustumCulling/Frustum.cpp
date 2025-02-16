#include "Frustum.h"

#include <iostream>

namespace Macecraft
{
    /**
     * <a href="https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html" >More details here (Bremen University)</a>
     */
    void Frustum::UpdateFromCamera(const Camera& camera)
{
    float aspect = camera.GetAspectRatio();
    const float hfar = 2 * camera.farPlane * glm::tan(glm::radians(camera.fov * 0.5f));
    const float wfar = hfar * aspect;

    glm::vec3 right = glm::cross(camera.orientation, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 up = glm::cross(camera.orientation, -right);

    glm::vec3 vectorPartOfRightPlane = glm::normalize(camera.orientation * camera.farPlane + wfar / 2 * right);
    glm::vec3 vectorPartOfLeftPlane = glm::normalize(camera.orientation * camera.farPlane - wfar / 2 * right);
    glm::vec3 vectorPartOfTopPlane = glm::normalize(camera.orientation * camera.farPlane + hfar / 2 * up);
    glm::vec3 vectorPartOfBottomPlane = glm::normalize(camera.orientation * camera.farPlane - hfar / 2 * up);

    glm::vec3 rightPlaneNormal = glm::cross(vectorPartOfRightPlane, -up);
    glm::vec3 leftPlaneNormal = glm::cross(vectorPartOfLeftPlane, up);
    glm::vec3 topPlaneNormal = glm::cross(vectorPartOfTopPlane, -right);
    glm::vec3 bottomPlaneNormal = glm::cross(vectorPartOfBottomPlane, right);

    nearPlane = { camera.orientation, glm::dot(camera.position, camera.orientation) + camera.nearPlane };
    rightPlane = { rightPlaneNormal, glm::dot(camera.position, rightPlaneNormal) };
    leftPlane = { leftPlaneNormal, glm::dot(camera.position, leftPlaneNormal) };
}


} // Macecraft