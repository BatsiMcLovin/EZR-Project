#version 450

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

out vec4 color;
out int ID;

uniform mat4 modelMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);


void main() 
{
color = vec4(0.5);
passPosition_w = (viewMatrix * modelMatrix * positionAttribute).xyz;
passNormal_w = (transpose(inverse(viewMatrix * modelMatrix)) * normalAttribute).xyz;
ID = gl_VertexID;

gl_Position = projectionMatrix * viewMatrix * modelMatrix * positionAttribute;

}
