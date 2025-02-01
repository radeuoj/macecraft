#pragma once

#include <glm/glm.hpp>

#include "VAO.h"

class Sprite
{
public:
    Sprite(glm::vec3 position, glm::vec2 size);

    void Render();
private:
    glm::vec3 position;
    glm::vec2 size;
};
