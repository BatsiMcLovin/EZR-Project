#version 450

layout (location = 0) in vec4 positionAttribute;
layout (location = 1) in vec2 uvAttribute;
layout (location = 2) in vec4 normalAttribute;
layout (location = 3) in vec4 tangentAttribute;
layout (location = 4) in vec4 bitangentAttribute;

layout (std430, binding = 0) buffer mySSBO
{
	int genericSSBOdata[][40];
};

/*
*/
layout(std430, binding = 2) restrict coherent buffer Positions
{
    vec4 positions[];
};

layout(std430, binding = 3) restrict coherent buffer Normals
{
    vec4 normals[];
};

out vec3 passPosition_w;
out vec2 passUV;
out vec3 passNormal_w;
out vec3 passTangent_w;
out vec3 passBitangent_w;
out vec4 ssboContents;

uniform mat4 modelMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

void main() {

passPosition_w = (modelMatrix * positionAttribute).xyz;
passNormal_w = (transpose(inverse(modelMatrix)) * normalAttribute).xyz;

passUV = uvAttribute;

gl_Position = projectionMatrix * viewMatrix * modelMatrix * positionAttribute;

float neiNum = (genericSSBOdata[gl_VertexID][39]);

if(length(positions[gl_VertexID]) < 0.01f)
neiNum *= 0.9f;

vec4 color = normals[gl_VertexID];

ssboContents = abs(color) * 0.5f + 0.5f;

ssboContents *= vec4(neiNum / 20.0f);
}
