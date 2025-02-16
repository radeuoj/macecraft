#pragma once

#include "Line.h"
#include "../Camera.h"

namespace Macecraft {

struct Frustum
{
    Line nearPlane, rightPlane, leftPlane;

    void UpdateFromCamera(const Camera& camera);
};

} // Macecraft
