#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Macecraft
{

struct Plane
{

    glm::vec3 normal = glm::vec3(0.0f);
    float distance = 0.0f;

    void Normalize()
    {
        float length = glm::length(normal);
        normal /= length;
        distance /= length;
    }

};

} // Macecraft
