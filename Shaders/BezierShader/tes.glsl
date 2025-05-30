#version 450 core
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
}