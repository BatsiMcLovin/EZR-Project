#version 450

layout (location = 0) in vec4 positionAttribute;
layout (location = 1) in vec2 uvAttribute;
layout (location = 2) in vec4 normalAttribute;
layout (location = 3) in vec4 tangentAttribute;
layout (location = 4) in vec4 bitangentAttribute;

out vec3 passPosition_w;
out vec2 passUV;
out vec3 passNormal_w;
out vec3 passTangent_w;
out vec3 passBitangent_w;

uniform mat4 modelMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

void main() {

passPosition_w = (modelMatrix * positionAttribute).xyz;
passNormal_w = (transpose(inverse(modelMatrix)) * normalAttribute).xyz;

passUV = uvAttribute;

gl_Position = projectionMatrix * viewMatrix * modelMatrix * positionAttribute;

}
