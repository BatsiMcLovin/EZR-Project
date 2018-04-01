#version 450

layout(location = 0) in vec4 positionAttribute;
layout(location = 1) in vec2 uvAttribute;

out vec2 passUV;

void main()
{	
	passUV = uvAttribute;
	gl_Position = positionAttribute;
}
