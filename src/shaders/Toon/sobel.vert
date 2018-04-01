#version 330 core

layout (location = 0) in vec4 positionAttribute;
layout (location = 2) in vec4 normalAttribute;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {

gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4((positionAttribute + normalAttribute * 0.1).xyz, 1.0);


}