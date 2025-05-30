#pragma once

#define SHADER_SHADERNAME_ControlPointShader "ControlPointShader"
#define LOAD_SHADER_ControlPointShader "ControlPointShader", ControlPointShader_vertexShader, ControlPointShader_tcsShader, ControlPointShader_tesShader, ControlPointShader_geometryShader, ControlPointShader_fragmentShader

const char* ControlPointShader_vertexShader = R"(#version 450 core
layout (location = 0) in vec2 vertexPos;
void main()
{
    gl_Position = vec4(vertexPos, 0, 1.0);
}
)";

const char* ControlPointShader_tcsShader = NULL;

const char* ControlPointShader_tesShader = NULL;

const char* ControlPointShader_geometryShader = R"(#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 uResolution; // Screen size
uniform float size;

void main(){
    vec2 scaledSize = vec2(size * (1 / uResolution.x), size * (1 / uResolution.y));

    vec4 worldPos = model * gl_in[0].gl_Position;
    vec4 pos = projection * view * worldPos; 
    vec2 CenterPoint = pos.xy / pos.w;
    
    vec2 offsetX = normalize(vec2(size, 0) / normalize(uResolution)) * scaledSize;
    vec2 offsetY = normalize(vec2(0, size) / normalize(uResolution)) * scaledSize;

    gl_Position = vec4(CenterPoint + offsetX + offsetY, pos.zw);
    EmitVertex();
    gl_Position = vec4(CenterPoint - offsetX + offsetY, pos.zw);
    EmitVertex();
    gl_Position = vec4(CenterPoint + offsetX - offsetY, pos.zw);
    EmitVertex();
    gl_Position = vec4(CenterPoint - offsetX - offsetY, pos.zw);
    EmitVertex();
    EndPrimitive();
})";

const char* ControlPointShader_fragmentShader = R"(#version 450 core
out vec4 FragColor;

uniform vec2 uResolution; // Screen size

void main()
{
    FragColor = vec4(0.5);
})";

