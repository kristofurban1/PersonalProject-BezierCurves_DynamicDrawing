#version 450 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    vec4 worldPos = model * vec4(aPos, 0, 1.0);
    vec4 clipPos = projection * view * worldPos;
    gl_Position = clipPos;
}
