#include "block.h"

#include "shader.h"

Block::Block(glm::vec3 position, float length): vertices
{
    position.x, position.y, position.z,  1.0f,  0.0f,
    length, 0.0f, 0.0f,  0.0f,  0.0f, 
    length, length, 0.0f,  0.0f,  1.0f,
    0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    length, length, 0.0f,  0.0f,  1.0f,
    0.0f, length, 0.0f,  1.0f,  1.0f,
    
    0.0f, 0.0f, 0.0f,  0.0f,  0.0f, 
    length, 0.0f, 0.0f,  1.0f,  0.0f,
    length, 0.0f, length,  1.0f,  1.0f,
    0.0f, 0.0f, 0.0f,  0.0f,  0.0f, 
    length, 0.0f, length,  1.0f,  1.0f,
    0.0f, 0.0f, length,  0.0f,  1.0f,
    
    0.0f, 0.0f, length,  0.0f,  0.0f, 
    length, 0.0f, length,  1.0f,  0.0f,
    length, length, length,  1.0f,  1.0f,
    0.0f, 0.0f, length,  0.0f,  0.0f, 
    length, length, length,  1.0f,  1.0f,
    0.0f, length, length,  0.0f,  1.0f,
    
    0.0f, length, 0.0f,  0.0f,  0.0f, 
    length, length, 0.0f,  1.0f,  0.0f,
    length, length, length,  1.0f,  1.0f,
    0.0f, length, 0.0f,  0.0f,  0.0f, 
    length, length, length,  1.0f,  1.0f,
    0.0f, length, length,  0.0f,  1.0f,
    
    0.0f, 0.0f, 0.0f,  0.0f,  0.0f, 
    0.0f, 0.0f, length,  1.0f,  0.0f,
    0.0f, length, length,  1.0f,  1.0f,
    0.0f, 0.0f, 0.0f,  0.0f,  0.0f, 
    0.0f, length, length,  1.0f,  1.0f,
    0.0f, length, 0.0f,  0.0f,  1.0f,
    
    length, 0.0f, 0.0f,  1.0f,  0.0f,
    length, 0.0f, length,  0.0f,  0.0f, 
    length, length, length,  0.0f,  1.0f,
    length, 0.0f, 0.0f,  1.0f,  0.0f,
    length, length, length,  0.0f,  1.0f,
    length, length, 0.0f,  1.0f,  1.0f,
}
{

}

void Block::Render()
{

}


