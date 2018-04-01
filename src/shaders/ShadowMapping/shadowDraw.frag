#version 330 core

//uniform vec3 diffuseColor;

layout(location = 0) out vec4 positionOutput;

in vec3 passPosition_w;

void main()
{

gl_FragColor = vec4(passPosition_w, 1.0);

}