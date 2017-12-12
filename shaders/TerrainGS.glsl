#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

uniform mat4 inWorldToCamera = mat4(1);
uniform mat4 inCameraToClip = mat4(1);

struct TERRAIN_CONFIG{
    float inMaxHeight;
    int inNumOctaves;
    float inPersistence;
    float inLacunarity;
    float inFrequency;
    float inExp;
    float inFlatToMidStart;
    float inFlatToMidEnd;
    float inMidToSlopeStart;
    float inMidToSlopeEnd ;
    float inHeightTexFadeStart;
    float inHeightTexFadeEnd;
    float inTextureID;
};

in struct TES_GS{
    vec2 texCoord;
    vec3 normal;
    TERRAIN_CONFIG terrainConfig;
} esToGS[3];

out struct GS_FS{
    vec3 vertWorldPos;
    vec2 texCoord;
    vec3 normal;
} gsToFS;

flat out TERRAIN_CONFIG terrainConfig;

void main(){
    mat4 worldToClip = inWorldToCamera * inCameraToClip;

    for(int vertIdx = 0; vertIdx < 3; vertIdx++){
        gsToFS.texCoord = esToGS[vertIdx].texCoord;
        gsToFS.normal = esToGS[vertIdx].normal;
        terrainConfig = esToGS[vertIdx].terrainConfig;
        gsToFS.vertWorldPos = gl_in[vertIdx].gl_Position.xyz;
        gl_Position = gl_in[vertIdx].gl_Position * worldToClip;
        EmitVertex();
    }

    EndPrimitive();
}
