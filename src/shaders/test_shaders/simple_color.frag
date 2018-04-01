#version 330 core

uniform vec3 color;
in vec3 passPosition;
in vec3 passNormal;
layout(location = 0) out vec4 positionOutput;
layout(location = 1) out vec4 normalOutput;

void main(){

float d = dot(normalize(-passPosition.xyz), normalize(passNormal));

d = (d + 1) / 2;
d = 1 - d;
d = d * d * 0.75f;

normalOutput = vec4(normalize(passNormal), 1);
positionOutput = vec4(color, 1) + vec4(d,d,d,1);
}