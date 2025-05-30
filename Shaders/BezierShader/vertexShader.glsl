#version 450 core
layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec2 controlPos;

out vec2 vControlPoints;

void main()
{
    gl_Position = vec4(vertexPos, 0, 1);
    vControlPoints = controlPos;
}
