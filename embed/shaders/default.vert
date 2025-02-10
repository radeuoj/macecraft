#version 330 core

layout (location = 0) in uvec3 aPos;
layout (location = 1) in int aTex;

//out vec3 color;
out vec2 texCoords;

uniform mat4 camMatrix;
uniform ivec2 chunkPos;

void main()
{
    gl_Position = camMatrix * vec4(vec3(chunkPos.x, 0.0, chunkPos.y) * 16 + vec3(aPos), 1.0);

    texCoords = vec2(aTex % 2, aTex / 2 % 2);
//    texCoords = vec2(float(aTex % 2) / 16, float(aTex / 2 % 2) / 16);
}