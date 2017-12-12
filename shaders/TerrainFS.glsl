#version 410 core

uniform int inTexCoordScale = 2;
uniform vec2 inCamPos = vec2(0, 0);
uniform float inFadeStart = 100;
uniform float inFadeTransition = 10;
uniform vec3 inFadeColour = vec3(0, 0, 0);
uniform vec3 inLightDirection = vec3(-1, -1, 0);

uniform struct TEXTURE_CONFIG{
    sampler2D inFlatTexture;
    sampler2D inMidTexture;
    sampler2D inSlopeTexture;
} inTextureConfig[2];

uniform sampler2D inHeightTexture;

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
    float inMidToSlopeEnd;
    float inHeightTexFadeStart;
    float inHeightTexFadeEnd;
    float inTextureID;
};

in struct GS_FS{
    vec3 vertWorldPos;
    vec2 texCoord;
    vec3 normal;
} gsToFS;

flat in TERRAIN_CONFIG terrainConfig;

out vec4 outputColour;

vec4 getSlopeBasedTextureAt(int index){
    float slopeFactor = 1.0 - dot(vec3(0, 1, 0), gsToFS.normal);
    vec4 textureColour;
    vec2 texCoord = gsToFS.texCoord*inTexCoordScale;
    float inFlatToMidStart = terrainConfig.inFlatToMidStart;
    float inFlatToMidEnd = terrainConfig.inFlatToMidEnd;
    float inMidToSlopeStart = terrainConfig.inMidToSlopeStart;
    float inMidToSlopeEnd = terrainConfig.inMidToSlopeEnd;
    if (slopeFactor < inFlatToMidStart)
        textureColour = texture(inTextureConfig[index].inFlatTexture, texCoord);
    else if (slopeFactor < inFlatToMidEnd)
        textureColour = mix(texture(inTextureConfig[index].inFlatTexture, texCoord), texture(inTextureConfig[index].inMidTexture, texCoord), (slopeFactor - inFlatToMidStart)/(inFlatToMidEnd - inFlatToMidStart));
    else if (slopeFactor < inMidToSlopeStart)
        textureColour = texture(inTextureConfig[index].inMidTexture, texCoord);
    else if (slopeFactor < inMidToSlopeEnd)
        textureColour = mix(texture(inTextureConfig[index].inMidTexture, texCoord), texture(inTextureConfig[index].inSlopeTexture, texCoord), (slopeFactor - inMidToSlopeStart)/(inMidToSlopeEnd - inMidToSlopeStart));
    else
        textureColour = texture(inTextureConfig[index].inSlopeTexture, texCoord);
    return textureColour;
}

vec4 getSlopeBasedTexture(){
    vec4 textureColour;

    if (terrainConfig.inTextureID < 0.001){
        textureColour = getSlopeBasedTextureAt(0);
    }
    else if (terrainConfig.inTextureID < 0.999){
        vec4 texture1 = getSlopeBasedTextureAt(0);
        vec4 texture2 = getSlopeBasedTextureAt(1);
        textureColour = mix(texture1, texture2, terrainConfig.inTextureID);
    }
    else{
        textureColour = getSlopeBasedTextureAt(1);
    }

    return textureColour;
}

vec4 getHeightBasedTexture(vec4 baseColour){
    vec4 textureColour;
    float inHeightTexFadeStart = terrainConfig.inHeightTexFadeStart;
    float inHeightTexFadeEnd = terrainConfig.inHeightTexFadeEnd;

    int index = 0;
    float factor = (gsToFS.vertWorldPos.y - inHeightTexFadeStart)/(inHeightTexFadeEnd - inHeightTexFadeStart);
    factor = clamp(factor, 0, 1);
    textureColour = mix(baseColour, texture(inHeightTexture, gsToFS.texCoord*inTexCoordScale), factor);

    return textureColour;
}

void main(){
    float lightFactor = clamp(dot(normalize(-inLightDirection), gsToFS.normal), 0.2, 0.9);
	float fadeFactor = (distance(gsToFS.vertWorldPos.xz, inCamPos) - inFadeStart) / inFadeTransition;
	fadeFactor = clamp(fadeFactor, 0.0, 1.0);

    vec4 textureColour = getSlopeBasedTexture();

    textureColour = getHeightBasedTexture(textureColour);

	outputColour = mix(textureColour * lightFactor, vec4(inFadeColour, 1), fadeFactor);
}
