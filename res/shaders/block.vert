#version 330 core

layout (location = 0) in uvec3 aPos;
layout (location = 1) in int aTex;

//out vec3 color;
out vec3 texCoords;

uniform mat4 camMatrix;
uniform ivec2 chunkPos;

void main()
{
    gl_Position = camMatrix * vec4(vec3(chunkPos.x, 0.0, chunkPos.y) * 16 + vec3(aPos), 1.0);
    
    texCoords = vec3(aTex & 1, (aTex >> 1) & 1, aTex >> 2);

//    NOTE: texture atlas coords
//    texCoords = vec2(float(aTex & 15) * 0.0625, float(aTex >> 4) * 0.0625);
}