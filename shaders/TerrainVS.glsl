#version 410 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

uniform vec2 inTerrainPos = vec2(0, 0);

out struct VS_TCS {
    vec2 vertWorldPos;
    vec2 texCoord;
} vsToCS;


void main(){
	vsToCS.vertWorldPos = inPosition + inTerrainPos;
	vsToCS.texCoord = inTexCoord;
}
