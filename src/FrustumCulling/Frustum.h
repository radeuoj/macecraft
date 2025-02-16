#pragma once

#include "Plane.h"
#include "../Camera.h"

namespace Macecraft {

struct Frustum
{
    
    Plane nearPlane, rightPlane, leftPlane, topPlane, bottomPlane;
    
    void UpdateFromCamera(const Camera& camera);

};

} // Macecraft
