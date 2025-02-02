#pragma once

#include <glad/glad.h>

struct VertexData
{
    GLshort chunk_x;
    GLshort chunk_y;
    GLubyte x;
    GLubyte y;
    GLubyte z;
    GLubyte texCoords;
};