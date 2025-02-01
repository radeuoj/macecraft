#include "sprite.h"

#include <GL/gl.h>

Sprite::Sprite(glm::vec3 position, glm::vec2 size) : position(position), size(size)
{

}

void Sprite::Render()
{
    GLfloat vertices[] =
    {
        position.x,          position.y,          position.z, 0.0f, 0.0f,
        position.x + size.x, position.y,          position.z, 1.0f, 0.0f,
        position.x + size.x, position.y + size.y, position.z, 1.0f, 1.0f,
        position.x,          position.y,          position.z, 0.0f, 0.0f,
        position.x + size.x, position.y + size.y, position.z, 1.0f, 1.0f,
        position.x,          position.y + size.y, position.z, 0.0f, 1.0f,
    };



    glDrawArrays(GL_TRIANGLES, 0, 6);
}

