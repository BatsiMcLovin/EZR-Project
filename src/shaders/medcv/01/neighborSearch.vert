#version 450
layout(location = 0) in vec4 positionAttribute;
layout (location = 1) in vec2 uvAttribute;
layout (location = 2) in vec4 normalAttribute;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out int ID;
out vec2 passUV;
out vec4 passPosition;


layout(std430, binding = 2) coherent buffer Positions
{
    vec4 positions[];
};

layout(std430, binding = 3) coherent buffer Normals
{
    vec4 normals[];
};

void main()
{
	ID = gl_VertexID;
	gl_Position =  positionAttribute;

	memoryBarrier();
	positions[gl_VertexID] = positionAttribute;
	memoryBarrier();
	normals[gl_VertexID] = normalAttribute;
	memoryBarrier(); 
	
	passPosition = positionAttribute;	
	passUV = uvAttribute;
}
