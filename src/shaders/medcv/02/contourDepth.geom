#version 450
#define max_expected_neighbors 40

layout(triangles) in;
in vec3 passPosition_w[];
in vec3 passNormal_w[];
in int ID[];

layout (triangle_strip, max_vertices = 4) out;

out vec4 color;

uniform mat4 projectionMatrix = mat4(1.0);
uniform float lineWidth = 0.01f;
uniform float lineFlatness = 0.0f;

// ------------------

vec3 getZeroCrossing(vec3 p1, vec3 p2, float r1, float r2)
{
    float x = -r2 / (r1 - r2);
    vec3 p = x * p1 + (1 - x) * p2;
    return p;
}

void drawOriginalTriangle(vec3 p0, vec3 p1, vec3 p2)
{
	color = vec4(1);
	gl_Position = projectionMatrix * vec4(p0, 1);
	EmitVertex();

	color = vec4(1);
	gl_Position = projectionMatrix * vec4(p1, 1);
	EmitVertex();

	color = vec4(1);
	gl_Position = projectionMatrix * vec4(p2, 1);
	EmitVertex();

	EndPrimitive();
}

void drawQuad(vec3 p0, vec3 p1, vec3 n0, vec3 n1)
{

	float width = lineWidth;

	color = abs(vec4(0));
	gl_Position = projectionMatrix * vec4(p0, 1);
	EmitVertex();

	color = abs(vec4(0));
	gl_Position = projectionMatrix * vec4(p1, 1);
	EmitVertex();

	color = abs(vec4(0));
	gl_Position = projectionMatrix * vec4(p0 + n0 * width, 1);
	EmitVertex();

	color = abs(vec4(0));
	gl_Position = projectionMatrix * vec4(p1 + n1 * width, 1);
	EmitVertex();

	EndPrimitive();
}

void main()
{
	int id0 = ID[0];
	
	// POSITIONS
	vec3 pos0 = passPosition_w[0];
	vec3 pos1 = passPosition_w[1];
	vec3 pos2 = passPosition_w[2];

	// NORMALS
	vec3 norm0 = passNormal_w[0];
	vec3 norm1 = passNormal_w[1];
	vec3 norm2 = passNormal_w[2];


	// DOT PRODUCTS

	vec3 pos0_norm = normalize(pos0);
	vec3 pos1_norm = normalize(pos1);
	vec3 pos2_norm = normalize(pos2);

	vec3 forward = vec3(0,0,1);
	float l = -lineFlatness;

	pos0_norm = mix(pos0_norm, forward, l);
	pos1_norm = mix(pos1_norm, forward, l);
	pos2_norm = mix(pos2_norm, forward, l);


	float d0 = dot(pos0_norm, normalize(norm0));
	float d1 = dot(pos1_norm, normalize(norm1));
	float d2 = dot(pos2_norm, normalize(norm2));
	
	// BORDERS

    bool b01 = d0 * d1 < 0;
    bool b12 = d1 * d2 < 0;
    bool b20 = d2 * d0 < 0;


	if(b01 && b12 )
	{
		vec3 p01 = getZeroCrossing(pos0, pos1, d0, d1);
		vec3 p12 = getZeroCrossing(pos1, pos2, d1, d2); 		

		vec3 n01 = getZeroCrossing(norm0, norm1, d0, d1);
		vec3 n12 = getZeroCrossing(norm1, norm2, d1, d2);

		n01 = normalize(n01);
		n12 = normalize(n12); 
	
		drawQuad(p01, p12, n01, n12);
	}	
	if(b12 && b20 )
	{
		vec3 p12 = getZeroCrossing(pos1, pos2, d1, d2);
		vec3 p20 = getZeroCrossing(pos2, pos0, d2, d0);

		vec3 n12 = getZeroCrossing(norm1, norm2, d1, d2);
		vec3 n20 = getZeroCrossing(norm2, norm0, d2, d0);

		n12 = normalize(n12);
		n20 = normalize(n20); 
	
		drawQuad(p12, p20, n12, n20);
	}
	if(b20 && b01 )
	{
		vec3 p20 = getZeroCrossing(pos2, pos0, d2, d0);
		vec3 p01 = getZeroCrossing(pos0, pos1, d0, d1);

		vec3 n20 = getZeroCrossing(norm2, norm0, d2, d0);
		vec3 n01 = getZeroCrossing(norm0, norm1, d0, d1);
		
		n20 = normalize(n20); 
		n01 = normalize(n01);	

		drawQuad(p20, p01, n20, n01);
	}	

	drawOriginalTriangle(pos0, pos1, pos2);


}
