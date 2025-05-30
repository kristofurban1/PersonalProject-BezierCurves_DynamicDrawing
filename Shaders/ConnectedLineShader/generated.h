#pragma once

#define SHADER_SHADERNAME_ConnectedLineShader "ConnectedLineShader"
#define LOAD_SHADER_ConnectedLineShader "ConnectedLineShader", ConnectedLineShader_vertexShader, ConnectedLineShader_tcsShader, ConnectedLineShader_tesShader, ConnectedLineShader_geometryShader, ConnectedLineShader_fragmentShader

const char* ConnectedLineShader_vertexShader = R"(#version 450 core
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
)";

const char* ConnectedLineShader_tcsShader = NULL;

const char* ConnectedLineShader_tesShader = NULL;

const char* ConnectedLineShader_geometryShader = R"(#version 450 core
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;


uniform vec2 uResolution; // Screen size
uniform float thickness;

void DrawLine(int inx1, int idx2){
    // Normalize clip position
    vec2 a = gl_in[inx1].gl_Position.xy / gl_in[inx1].gl_Position.w;
    vec2 b = gl_in[idx2].gl_Position.xy / gl_in[idx2].gl_Position.w;
    // Calculate perpendicular vector
    vec2 dir = normalize(b-a);
    vec2 normalVector = vec2(-dir.y, dir.x);

    // Scale the thickness offset 
    //vec2 aspectRatioVector = normalize(uResolution);
    //vec2 scaledThickness = vec2(thickness) / aspectRatioVector;

    vec2 scaledThickness = vec2(thickness * (1 / uResolution.x), thickness * (1 / uResolution.y));
    vec2 normalOffset = normalize(normalVector / normalize(uResolution)) * scaledThickness;
    
    
    gl_Position = gl_in[inx1].gl_Position + vec4( normalOffset, 0, 0);
    EmitVertex();
    gl_Position = gl_in[inx1].gl_Position + vec4(-normalOffset, 0, 0);
    EmitVertex();
    gl_Position = gl_in[idx2].gl_Position + vec4( normalOffset, 0, 0);
    EmitVertex();
    gl_Position = gl_in[idx2].gl_Position + vec4(-normalOffset, 0, 0);
    EmitVertex();
    EndPrimitive();
}

void main(){
    DrawLine(0,1);
})";

const char* ConnectedLineShader_fragmentShader = R"(#version 450 core
uniform vec2 uResolution; // Screen size
out vec4 FragColor;


void main()
{
    FragColor = vec4(0.8,0,0,1);
})";

