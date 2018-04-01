#version 450

layout(location = 0) out vec4 fragcolor;
layout(location = 1) out vec4 fragcolorOutPos;

in vec4 outDebug;
in vec4 outposition;

void main()
{
	fragcolor = outDebug;
	fragcolorOutPos = outposition;
}
