#version 330 core

out vec4 FragColor;

//in vec3 color;
in vec3 texCoords;

uniform sampler2DArray tex0;

void main()
{
    FragColor = texture(tex0, texCoords);
}