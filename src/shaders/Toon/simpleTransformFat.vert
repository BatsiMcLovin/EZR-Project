#version 330 core

layout (location = 0) in vec4 positionAttribute;
layout (location = 1) in vec2 uvAttribute;
layout (location = 2) in vec4 normalAttribute;
layout (location = 3) in vec4 tangentAttribute;
layout (location = 4) in vec4 bitangentAttribute;

out vec3 passPosition;
out vec2 passUV;
out vec3 passNormal;
out vec3 passTangent;
out vec3 passBitangent;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {

passPosition = (viewMatrix * modelMatrix * positionAttribute).xyz;
passNormal = (transpose(inverse(viewMatrix * modelMatrix)) * normalAttribute).xyz;

passUV = uvAttribute;

gl_Position = projectionMatrix * viewMatrix * modelMatrix * positionAttribute;


}