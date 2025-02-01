#version 330 core

out vec4 FragColor;

//in vec3 color;
in vec3 texCoords;

uniform samplerCube cubemap;

void main()
{
    FragColor = texture(cubemap, texCoords);
}