#version 430
layout(location = 0) in vec4 positionAttribute;
layout(location = 1) in vec2 uvCoordAttribute;
layout(location = 2) in vec4 normalAttribute;

out vec4 Vertex_geom;
out vec4 Normal_geom;

void main()
{
  Vertex_geom = positionAttribute;
  Normal_geom = normalAttribute;
}
