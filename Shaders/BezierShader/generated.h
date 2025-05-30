#pragma once

#define SHADER_SHADERNAME_BezierShader "BezierShader"
#define LOAD_SHADER_BezierShader "BezierShader", BezierShader_vertexShader, BezierShader_tcsShader, BezierShader_tesShader, BezierShader_geometryShader, BezierShader_fragmentShader

const char* BezierShader_vertexShader = R"(#version 450 core
layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec2 controlPos;

out vec2 vControlPoints;

void main()
{
    gl_Position = vec4(vertexPos, 0, 1);
    vControlPoints = controlPos;
}
)";

const char* BezierShader_tcsShader = R"(#version 450 core

layout (vertices=2) out;

uniform int tess1;
uniform int isValid;

in vec2 vControlPoints[];
out vec2 tcsEndPoints[];
out vec2 tcsControlPoints[];

const vec2 AutoSegemterParams = vec2(8, 12);

float estimateLength(vec2 a, vec2 b, vec2 c, vec2 d){
    float ab = distance(a, b);
    float bc = distance(b, c);
    float cd = distance(c, d);
    return ab+bc+cd;
}

// sqrt(-(pow(x-c, 2) - pow(((c*c) / (a*a)) * (x - (a*a)/c), 2)))
float segmentCount(float estimatedLength){
    int a = int(AutoSegemterParams.x);
    int c = int(AutoSegemterParams.y);
    float x = estimatedLength;

    float p1 = ((c*c) / (a*a)) * (x - (a*a)/c);
    float p2 = x-c;
    float p3 = p2*p2 - p1*p1;
    return sqrt(-p3);
}

void main(){
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcsEndPoints[gl_InvocationID]       = gl_in[gl_InvocationID].gl_Position.xy;
    tcsControlPoints[gl_InvocationID]   = vControlPoints[gl_InvocationID];
    
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 1;

        float estimatedLength = estimateLength(
            gl_in[0].gl_Position.xy,
            vControlPoints[0].xy,
            vControlPoints[1].xy,
            gl_in[1].gl_Position.xy
        );

        int tessellation = max(int(ceil(segmentCount(estimatedLength))), 8);

        gl_TessLevelOuter[1] = tessellation;
    }
})";

const char* BezierShader_tesShader = R"(#version 450 core
layout (isolines, equal_spacing, cw) in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec2 tcsEndPoints[];
in vec2 tcsControlPoints[];

const float dt = 0.01;
out vec2 tangent;

vec2 BezierCurve(float t){
    float y = 1-t;
    vec2 p0 = y*y*y * tcsEndPoints[0];
    vec2 p1 = 3 * y*y * t * tcsControlPoints[0];
    vec2 p2 = 3 * y * t*t * tcsControlPoints[1];
    vec2 p3 = t*t*t * tcsEndPoints[1];

    return p0+p1+p2+p3;
}

void main(){
    float u = gl_TessCoord.x;

    vec2 curve = BezierCurve(u);
    vec4 pos = vec4(curve, 0, 1);
    vec4 worldPos = model * pos;
    gl_Position = projection * view * worldPos; 
    

    vec2 dcurve = BezierCurve(u + dt) - BezierCurve(u - dt);
    vec2 tangentVector = dcurve / (2*dt);
    tangent = normalize(tangentVector.xy);
})";

const char* BezierShader_geometryShader = R"(#version 450 core
layout (lines) in;
layout (triangle_strip, max_vertices = 8) out;

uniform vec2 uResolution; // Screen size
uniform float thickness;
in vec2 tangent[];

vec2 RotateCCW(vec2 v){
    return vec2(-v.y, v.x);
}


void DrawLine(int idxA, int idxB){
    vec2 scaledThickness = vec2(thickness * (1 / uResolution.x), thickness * (1 / uResolution.y));

    // Normalize clip position
    vec2 a = gl_in[idxA].gl_Position.xy / gl_in[idxA].gl_Position.w;
    vec2 b = gl_in[idxB].gl_Position.xy / gl_in[idxB].gl_Position.w;

    // Calculate perpendicular vector
    vec2 dir = normalize(b-a);
    vec2 normalVector = RotateCCW(dir);

    // Scale the thickness offset 
    vec2 _normalOffset = normalize(normalVector / normalize(uResolution)) * scaledThickness;
    vec4 normalOffset = vec4(_normalOffset, 0, 0);
    
    vec2 tangentA = tangent[0];
    vec2 normalTanA = RotateCCW(tangentA);
    vec4 tangentAOffset = vec4(normalTanA * scaledThickness, 0, 0);

    vec2 tangentB = tangent[1];
    vec2 normalTanB = RotateCCW(tangentB);
    vec4 tangentBOffset =  vec4(normalTanB * scaledThickness, 0, 0);

    /*
    0   1
    2   3

    4   5
    6   7
    */


    
    gl_Position = gl_in[idxA].gl_Position - tangentAOffset;
    EmitVertex();
    gl_Position = gl_in[idxA].gl_Position + tangentAOffset;
    EmitVertex();

    gl_Position = gl_in[idxA].gl_Position - normalOffset;
    EmitVertex();
    gl_Position = gl_in[idxA].gl_Position + normalOffset;
    EmitVertex();

    gl_Position = gl_in[idxB].gl_Position - normalOffset;
    EmitVertex();
    gl_Position = gl_in[idxB].gl_Position + normalOffset;
    EmitVertex();

    gl_Position = gl_in[idxB].gl_Position - tangentBOffset;
    EmitVertex();
    gl_Position = gl_in[idxB].gl_Position + tangentBOffset;
    EmitVertex();
    EndPrimitive();
}

void main(){
    DrawLine(0, 1);
})";

const char* BezierShader_fragmentShader = R"(#version 450 core
out vec4 FragColor;


void main()
{
    FragColor = vec4(1);
})";

