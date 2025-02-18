#include "Frustum.h"

#include <iostream>

namespace Macecraft
{
/**
 * <a href="https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html" >More details here (Bremen University)</a>
 * Do not use, it does not really work
 */
void Frustum::UpdateFromCamera(const Camera& camera)
{
    float aspect = camera.GetAspectRatio();
    const float hfar = 2 * camera.FAR_PLANE * glm::tan(glm::radians(camera.FOV * 0.5f));
    const float wfar = hfar * aspect;

    glm::vec3 right = glm::cross(camera.orientation, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 up = glm::cross(camera.orientation, -right);

    glm::vec3 vectorPartOfRightPlane = glm::normalize(camera.orientation * camera.FAR_PLANE + wfar / 2 * right);
    glm::vec3 vectorPartOfLeftPlane = glm::normalize(camera.orientation * camera.FAR_PLANE - wfar / 2 * right);
    glm::vec3 vectorPartOfTopPlane = glm::normalize(camera.orientation * camera.FAR_PLANE + hfar / 2 * up);
    glm::vec3 vectorPartOfBottomPlane = glm::normalize(camera.orientation * camera.FAR_PLANE - hfar / 2 * up);

    glm::vec3 rightPlaneNormal = glm::cross(vectorPartOfRightPlane, -up);
    glm::vec3 leftPlaneNormal = glm::cross(vectorPartOfLeftPlane, up);
    glm::vec3 topPlaneNormal = glm::cross(vectorPartOfTopPlane, -right);
    glm::vec3 bottomPlaneNormal = glm::cross(vectorPartOfBottomPlane, right);

    nearPlane = { camera.orientation, glm::dot(camera.position, camera.orientation) + camera.NEAR_PLANE };
    rightPlane = { rightPlaneNormal, glm::dot(camera.position, rightPlaneNormal) };
    leftPlane = { leftPlaneNormal, glm::dot(camera.position, leftPlaneNormal) };
}

/**
 * https://github.com/copilot/c/3e107979-5a02-47b6-9888-9053ee335ee0
 */
void Frustum::UpdateFromViewProjMatrix(const glm::mat4& viewproj)
{
    // Left
    leftPlane.normal.x = viewproj[0][3] + viewproj[0][0];
    leftPlane.normal.y = viewproj[1][3] + viewproj[1][0];
    leftPlane.normal.z = viewproj[2][3] + viewproj[2][0];
    leftPlane.distance = viewproj[3][3] + viewproj[3][0];
    leftPlane.Normalize();

    // Right
    rightPlane.normal.x = viewproj[0][3] - viewproj[0][0];
    rightPlane.normal.y = viewproj[1][3] - viewproj[1][0];
    rightPlane.normal.z = viewproj[2][3] - viewproj[2][0];
    rightPlane.distance = viewproj[3][3] - viewproj[3][0];
    rightPlane.Normalize();

    // Bottom
    bottomPlane.normal.x = viewproj[0][3] + viewproj[0][1];
    bottomPlane.normal.y = viewproj[1][3] + viewproj[1][1];
    bottomPlane.normal.z = viewproj[2][3] + viewproj[2][1];
    bottomPlane.distance = viewproj[3][3] + viewproj[3][1];
    bottomPlane.Normalize();

    // Top
    topPlane.normal.x = viewproj[0][3] - viewproj[0][1];
    topPlane.normal.y = viewproj[1][3] - viewproj[1][1];
    topPlane.normal.z = viewproj[2][3] - viewproj[2][1];
    topPlane.distance = viewproj[3][3] - viewproj[3][1];
    topPlane.Normalize();

    // Near
    nearPlane.normal.x = viewproj[0][3] + viewproj[0][2];
    nearPlane.normal.y = viewproj[1][3] + viewproj[1][2];
    nearPlane.normal.z = viewproj[2][3] + viewproj[2][2];
    nearPlane.distance = viewproj[3][3] + viewproj[3][2];
    nearPlane.Normalize();

    // Far
    farPlane.normal.x = viewproj[0][3] - viewproj[0][2];
    farPlane.normal.y = viewproj[1][3] - viewproj[1][2];
    farPlane.normal.z = viewproj[2][3] - viewproj[2][2];
    farPlane.distance = viewproj[3][3] - viewproj[3][2];
    farPlane.Normalize();

    GenerateChunkP();
}

void Frustum::GenerateChunkP()
{
    leftPlane.GenerateP();
    rightPlane.GenerateP();
    bottomPlane.GenerateP();
    topPlane.GenerateP();
    nearPlane.GenerateP();
    farPlane.GenerateP();
}


} // Macecraft