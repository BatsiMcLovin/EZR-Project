#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec4 passCurv_c_geom;
out vec4 passWorldVector_geom;
out vec4 passPosition_geom;
out vec4 passPosition_w_geom;
out vec4 passNormal_geom;
out vec4 passNormal_w_geom;
out vec3 passWorldSurfaceNormal_geom;
out float passDeriv_geom;

in vec4 passCurv_c[];
in vec4 passCurv[];
in vec4 passPosition_c[];
in vec4 passNormal_c[];
in vec4 passPosition_w[];
in vec4 passNormal_w[];
in float passDeriv[];

void main()
{
	// calc surface normal
	vec3 v1 = passPosition_w[1].xyz - passPosition_w[0].xyz;
	vec3 v2 = passPosition_w[2].xyz - passPosition_w[0].xyz;

	passWorldSurfaceNormal_geom = normalize(cross(v1,v2));

	// pass through
	for(int i = 0; i < gl_in.length(); i++)
	{
	    gl_Position = gl_in[i].gl_Position;
		passCurv_c_geom = passCurv_c[i];
		passWorldVector_geom = passCurv[i];
		passPosition_geom = passPosition_c[i];
		passNormal_geom = passNormal_c[i];
		passNormal_w_geom = passNormal_w[i];
		passPosition_w_geom = passPosition_w[i];
		passDeriv_geom = passDeriv[i];

	    EmitVertex();
	}
	EndPrimitive();
}
