#version 330

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

void main()
{

passPosition_w = positionAttribute.xyz;
passNormal_w = normalAttribute.xyz;
passUV = uvAttribute;
passTangent_w = tangentAttribute.xyz;
passBitangent_w = bitangentAttribute.xyz;

gl_Position = positionAttribute;

}