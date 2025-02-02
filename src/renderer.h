#pragma once

#include <array>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"

class Renderer
{
public:
    Renderer();

    /**
     * !!!!!!!!!!!!!!!!!!!!!!DO NOT USE!!!!!!!!!!!!
     */
    void RenderVertices(GLfloat vertices[], GLsizeiptr size);

    void RenderTriangleStrip(GLfloat vertices[], GLsizeiptr size);

    /**
     *
     * @param vertices MUST HAVE 4 VERTICES OF SIZE 5 GLfloats: 3 for coords, 2 for texcoords
     * @param texture the texture
     */
    void RenderSpriteVertices(GLfloat vertices[], Texture& texture);

    void RenderSprite(glm::vec3 position, glm::vec2 size, Texture& texture);

    void RenderBlock(glm::vec3 position, glm::vec3 scale, Texture& texture);

    void Flush();

    void Delete();

private:
    std::vector<GLfloat> spriteVertices;
    // std::vector<Texture*> spriteTextures;
    GLuint VAO{}, VBO{};
};
