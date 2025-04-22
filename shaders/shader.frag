#version 460 core
out vec3 FragColor;

uniform vec3 ourColor;

void main()
{
    FragColor = ourColor;
}