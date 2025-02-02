#version 330 core

#extension GL_NV_gpu_shader5 : enable

layout (location = 0) in i16vec2 aChunkPos;
layout (location = 1) in u8vec3 aPos;
layout (location = 2) in uint8_t aTex;
//layout (location = 1) in vec3 aColor;

//out vec3 color;
out vec2 texCoords;

uniform mat4 camMatrix;

void main()
{
//    gl_Position = vec4(aPos.x * scale, aPos.y * scale, aPos.z * scale, 1.0);
//    gl_Position = camMatrix * vec4((vec3(aChunkPos.x, 0.0, aChunkPos.y) * 16 + vec3(aPos.x, aPos.y, aPos.z)) * 0.5, 1.0);
//    vec3 faPos = vec3(float(aPos.x), float(aPos.y), float(aPos.z));
    gl_Position = camMatrix * vec4((vec3(aChunkPos.x, 0.0, aChunkPos.y) * 16 + vec3(aPos)) * 0.5, 1.0);
//    color = aColor;
    texCoords = vec2(int(aTex) % 2, int(aTex) / 2 % 2);
}