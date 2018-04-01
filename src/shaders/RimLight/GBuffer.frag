#version 330

in vec3 passPosition_c;
in vec2 passUV;
in vec3 passNormal_c;
in vec3 passTangent_c;
in vec3 passBitangent_c;

uniform vec3 diffuseColor;
float rimlightAmount;

layout(location = 0) out vec4 positionOutput;
layout(location = 1) out vec4 uvOutput;
layout(location = 2) out vec4 normalOutput;
layout(location = 3) out vec4 colorOutput;
layout(location = 4) out vec4 tangentOutput;
layout(location = 5) out vec4 bitangentOutput;
layout(location = 6) out vec4 rimlightAmountOutput;

void main() {

positionOutput = vec4(passPosition_c, 1);
uvOutput = vec4(passUV,0, 1);
normalOutput = vec4(normalize(passNormal_c), 1);
tangentOutput = vec4(passTangent_c, 1);
bitangentOutput = vec4(passBitangent_c, 1);

colorOutput = vec4(diffuseColor, 1);

rimlightAmountOutput = vec4(1.0f, 0.0f, 0.0f, 1.0f);

}