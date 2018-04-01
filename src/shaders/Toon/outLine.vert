#version 330

layout (location = 0) in vec4 positionAttribute;
layout (location = 2) in vec4 normalAttribute;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float thickness;

void main() {

gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4((positionAttribute + normalAttribute * thickness).xyz, 1.0);


}