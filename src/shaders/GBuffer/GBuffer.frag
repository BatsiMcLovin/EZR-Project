#version 330

in vec3 passPosition_w;
in vec2 passUV;
in vec3 passNormal_w;
in vec3 passTangent_w;
in vec3 passBitangent_w;

uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float specularHardness = 0.5;
uniform float reflectivity = 0.0;

uniform int materialID = 0;

layout(location = 0) out vec4 diffuseOutput;
layout(location = 1) out vec4 specularOutput;
layout(location = 2) out vec4 emissiveOutput;
layout(location = 3) out vec4 normalOutput;
layout(location = 4) out vec4 positionOutput;
layout(location = 5) out vec4 materialIDOutput;
layout(location = 6) out vec4 reflectivityOutput;

void main() {

emissiveOutput = vec4(1.0f);
positionOutput = vec4(passPosition_w, 1);
normalOutput = vec4(normalize(passNormal_w), 1);
materialIDOutput = vec4(materialID);
reflectivityOutput = (reflectivity != 1)? vec4(reflectivity) : vec4(0);
diffuseOutput = vec4(diffuseColor, 1);
specularOutput = vec4(specularColor, specularHardness);
}