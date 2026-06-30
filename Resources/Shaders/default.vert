#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 MVP;
uniform mat4 model;
out vec3 worldPos;
void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);
    worldPos = worldPosition.xyz;
    gl_Position = MVP * vec4(aPos, 1.0);
}