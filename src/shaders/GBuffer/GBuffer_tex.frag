#version 330

in vec3 passPosition_w;
in vec2 passUV;
in vec3 passNormal_w;
in vec3 passTangent_w;
in vec3 passBitangent_w;

uniform sampler2D diffuseColor;

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

vec3 transformToTangentSpace(vec3 vec)
{
    vec3 bitangent = cross(passNormal_w, passTangent_w);
    mat3 matrix = mat3(passTangent_w, bitangent, passNormal_w);

    return matrix * vec;
}

void main() {

positionOutput = vec4(passPosition_w, 1);
normalOutput = vec4(normalize(passNormal_w), 1);
materialIDOutput = vec4(materialID);
emissiveOutput = vec4(0.0f);
diffuseOutput = texture(diffuseColor, passUV);
specularOutput = vec4(specularColor, specularHardness);
}