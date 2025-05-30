#version 450 core
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
}