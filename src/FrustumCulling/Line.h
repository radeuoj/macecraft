#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Macecraft
{

struct Line
{

    glm::vec2 normal = glm::vec2(0.0f, 0.0f);
    float distance = 0.0f;

    float GetSignedDistanceToPlane(const glm::vec2 point) const
    {
        return glm::dot(normal, point) - distance;
    }

};

} // Macecraft
