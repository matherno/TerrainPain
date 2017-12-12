#version 410 core

layout(triangles, fractional_even_spacing, ccw) in;

uniform struct TERRAIN_CONFIG{
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
} inTerrainConfig[2];

in struct TCS_TES{
    vec2 vertWorldPos;
    vec2 texCoord;
} csToES[];

out struct TES_GS{
    vec2 texCoord;
    vec3 normal;
    TERRAIN_CONFIG terrainConfig;
} esToGS;

vec3 interpolateTri3D(vec3 v0, vec3 v1, vec3 v2){
    return v0 * gl_TessCoord.x + v1 * gl_TessCoord.y + v2 * gl_TessCoord.z;
}

vec2 interpolateTri2D(vec2 v0, vec2 v1, vec2 v2){
    return v0 * gl_TessCoord.x + v1 * gl_TessCoord.y + v2 * gl_TessCoord.z;
}

float random(vec2 seed){
	return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

float perlinNoise(vec2 pos){
    vec2 floorPos = floor(pos);
    vec2 fractPos = smoothstep(0.0, 1.0, fract(pos));

    float tlHeight = random(floorPos);
    float trHeight = random(floorPos + vec2(1, 0));
    float blHeight = random(floorPos + vec2(0, 1));
    float brHeight = random(floorPos + vec2(1, 1));

    return mix(tlHeight, trHeight, fractPos.x) +
        (blHeight - tlHeight) * fractPos.y * (1 - fractPos.x) +
        (brHeight - trHeight) * fractPos.x * fractPos.y;
}

vec2 rotate(vec2 vector, float angle) {
	float sinAngle = sin(angle);
	float cosAngle = cos(angle);
	mat2 rotation = mat2(cosAngle, sinAngle, -sinAngle, cosAngle);
	return vector * rotation;
}

float mixedNoise(vec2 pos){
    float pNoise1 = perlinNoise(pos);
    float pNoise2 = perlinNoise(rotate(pos, 3.0) + 950);
    float pNoise3 = perlinNoise(rotate(pos, 1.5) + 500);
    return (pNoise1 + pNoise2 + pNoise3) / 3.0;
}

float octavedNoise(vec2 pos){
    pos /= 10;
    float height = 0, amplitude = 1, frequency = 1;
    for(int octave = 0; octave < esToGS.terrainConfig.inNumOctaves; octave++){
        height += mixedNoise(pos * frequency) * amplitude;
        frequency *= esToGS.terrainConfig.inLacunarity;
        amplitude *= esToGS.terrainConfig.inPersistence;
        pos += random(vec2(octave*20+643, octave*99));
    }
    return height;
}

float getVertHeight(vec2 pos){
    return pow(octavedNoise(pos * esToGS.terrainConfig.inFrequency) * esToGS.terrainConfig.inMaxHeight, esToGS.terrainConfig.inExp);
}

const float offset = 0.5;

void setTerrainConfig(vec2 pos){
    float biomeFactor = mixedNoise(pos * 0.002);

//  use these values to determine the spread and interpolation amount of the two biomes (0 -> 1)
    float biome1End = 1;
    float biome2Start = 0.7;

    if (biomeFactor < biome1End)
        esToGS.terrainConfig = inTerrainConfig[0];
    else if (biomeFactor < biome2Start){
        float blendFactor = (biomeFactor - biome1End) / (biome2Start - biome1End);
        esToGS.terrainConfig.inMaxHeight            = mix(inTerrainConfig[0].inMaxHeight         , inTerrainConfig[1].inMaxHeight         , blendFactor);
        esToGS.terrainConfig.inNumOctaves           = int(mix(inTerrainConfig[0].inNumOctaves        , inTerrainConfig[1].inNumOctaves        , blendFactor));
        esToGS.terrainConfig.inPersistence          = mix(inTerrainConfig[0].inPersistence       , inTerrainConfig[1].inPersistence       , blendFactor);
        esToGS.terrainConfig.inLacunarity           = mix(inTerrainConfig[0].inLacunarity        , inTerrainConfig[1].inLacunarity        , blendFactor);
        esToGS.terrainConfig.inFrequency            = mix(inTerrainConfig[0].inFrequency         , inTerrainConfig[1].inFrequency         , blendFactor);
        esToGS.terrainConfig.inExp                  = mix(inTerrainConfig[0].inExp               , inTerrainConfig[1].inExp               , blendFactor);
        esToGS.terrainConfig.inFlatToMidStart       = mix(inTerrainConfig[0].inFlatToMidStart    , inTerrainConfig[1].inFlatToMidStart    , blendFactor);
        esToGS.terrainConfig.inFlatToMidEnd         = mix(inTerrainConfig[0].inFlatToMidEnd      , inTerrainConfig[1].inFlatToMidEnd      , blendFactor);
        esToGS.terrainConfig.inMidToSlopeStart      = mix(inTerrainConfig[0].inMidToSlopeStart   , inTerrainConfig[1].inMidToSlopeStart   , blendFactor);
        esToGS.terrainConfig.inMidToSlopeEnd        = mix(inTerrainConfig[0].inMidToSlopeEnd     , inTerrainConfig[1].inMidToSlopeEnd     , blendFactor);
        esToGS.terrainConfig.inHeightTexFadeStart   = mix(inTerrainConfig[0].inHeightTexFadeStart, inTerrainConfig[1].inHeightTexFadeStart, blendFactor);
        esToGS.terrainConfig.inHeightTexFadeEnd     = mix(inTerrainConfig[0].inHeightTexFadeEnd  , inTerrainConfig[1].inHeightTexFadeEnd  , blendFactor);
        esToGS.terrainConfig.inTextureID            = mix(inTerrainConfig[0].inTextureID         , inTerrainConfig[1].inTextureID         , blendFactor);
    }
    else
        esToGS.terrainConfig = inTerrainConfig[1];
}

void main() {
    vec2 interpPoint = interpolateTri2D(csToES[0].vertWorldPos, csToES[1].vertWorldPos, csToES[2].vertWorldPos);
    esToGS.texCoord = interpolateTri2D(csToES[0].texCoord, csToES[1].texCoord, csToES[2].texCoord);
    setTerrainConfig(interpPoint);

    float vertHeight = getVertHeight(interpPoint);
    vec3 vertWorldPos = vec3(interpPoint.x, vertHeight, interpPoint.y);
    gl_Position = vec4(vertWorldPos, 1);

    vec3 offsetX = vec3(offset, 0, 0) + vertWorldPos;
    offsetX.y = getVertHeight(offsetX.xz);
    vec3 offsetZ = vec3(0, 0, offset) + vertWorldPos;
    offsetZ.y = getVertHeight(offsetZ.xz);
    esToGS.normal = normalize(cross(vertWorldPos - offsetX, offsetZ - offsetX));
}
