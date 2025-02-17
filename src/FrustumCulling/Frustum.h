#pragma once

#include "Plane.h"
#include "../Camera.h"

namespace Macecraft {

struct Frustum
{
    
    Plane nearPlane, farPlane, rightPlane, leftPlane, topPlane, bottomPlane;

    [[deprecated]] void UpdateFromCamera(const Camera& camera);
    void UpdateFromViewProjMatrix(const glm::mat4& viewproj);

    void GenerateChunkP();

};

} // Macecraft
