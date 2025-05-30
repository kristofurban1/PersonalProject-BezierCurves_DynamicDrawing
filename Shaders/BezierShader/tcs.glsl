#version 450 core

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
}