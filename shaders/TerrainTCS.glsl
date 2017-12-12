#version 410 core

layout (vertices = 3) out;

const int DEFAULT_MIN_TESS_LEVEL = 4;
const int DEFAULT_MAX_TESS_LEVEL = 10;

uniform vec2 inCamPos = vec2(0, 0);
uniform float tessDistance = 700;
uniform int minTessLevel = DEFAULT_MIN_TESS_LEVEL;
uniform int maxTessLevel = DEFAULT_MAX_TESS_LEVEL;
uniform float tessNumSteps = DEFAULT_MAX_TESS_LEVEL - DEFAULT_MIN_TESS_LEVEL;

in struct VS_TCS {
    vec2 vertWorldPos;
    vec2 texCoord;
} vsToCS[];

out struct TCS_TES{
    vec2 vertWorldPos;
    vec2 texCoord;
} csToES[];

int calcTessLevel(float distance);

void main() {
    csToES[gl_InvocationID].vertWorldPos = vsToCS[gl_InvocationID].vertWorldPos;
    csToES[gl_InvocationID].texCoord = vsToCS[gl_InvocationID].texCoord;

    float distanceTo0 = distance(vsToCS[0].vertWorldPos, inCamPos);
    float distanceTo1 = distance(vsToCS[1].vertWorldPos, inCamPos);
    float distanceTo2 = distance(vsToCS[2].vertWorldPos, inCamPos);

    gl_TessLevelOuter[0] = calcTessLevel((distanceTo1 + distanceTo2) / 2.0);
    gl_TessLevelOuter[1] = calcTessLevel((distanceTo0 + distanceTo2) / 2.0);
    gl_TessLevelOuter[2] = calcTessLevel((distanceTo0 + distanceTo1) / 2.0);
    gl_TessLevelInner[0] = min(min(gl_TessLevelOuter[0], gl_TessLevelOuter[1]), gl_TessLevelOuter[2]);
}

int calcTessLevel(float distance){
    float distanceFactor = distance / tessDistance;
    distanceFactor = clamp(1.0-distanceFactor, 0.0, 1.0);
    int tessStepNum = int(ceil(distanceFactor * tessNumSteps));
    float tessStepSize = (maxTessLevel - minTessLevel) / tessNumSteps;
    return int(minTessLevel + tessStepNum*tessStepSize);
}
