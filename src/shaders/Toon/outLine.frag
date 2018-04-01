#version 330

layout(location = 0) out vec4 outLineOutput;

uniform vec4 sillouetteColor;


void main() {

outLineOutput = sillouetteColor;

}