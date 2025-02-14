#pragma once

#include "Line.h"
#include "../Camera.h"

namespace Macecraft {

struct Frustum
{
    Line nearPlane;

    void updateFromCamera(const Camera& camera);
};

} // Macecraft
