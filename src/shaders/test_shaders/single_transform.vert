#version 330 core

layout(location=0) in vec4 positionAttribute;
layout(location=1) in vec2 uvAttribute;
layout(location=2) in vec4 normalAttribute;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
out vec3 passPosition;
out vec2 passUV;
out vec3 passNormal;

void main(){
passNormal = (transpose(inverse(viewMatrix * modelMatrix))*normalAttribute).xyz;
passUV = uvAttribute;
passPosition= (viewMatrix * modelMatrix * positionAttribute).xyz;
gl_Position= projectionMatrix * viewMatrix * modelMatrix * positionAttribute;
}