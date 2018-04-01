#version 450
#define max_expected_neighbors 40

out vec4 passRadialCurvDeriv;

vec3 viewDirection = vec3(0, 0, 1);

uniform mat4 modelMatrix = mat4(1.0f);
uniform mat4 viewMatrix = mat4(1.0f);
uniform mat4 projectionMatrix = mat4(1.0f);

layout(std430, binding = 0) restrict buffer Nei
{
    int neighbors[][max_expected_neighbors];
};

layout(std430, binding = 2) restrict buffer Vert
{
    vec4 vertices[];
};

layout(std430, binding = 3) restrict buffer Nor
{
    vec4 normals[];
};

layout(std430, binding = 4) restrict buffer Curv
{
    vec4 curvature[];
};

layout(std430, binding = 5) restrict buffer Deriv
{
    float derivative[];
};

layout(location = 0) in vec4 positionAttribute;
layout(location = 2) in vec4 normalAttribute;

float computeRadialCurvatureDerivative()
{
    float result = 0;

	viewDirection = normalize((viewMatrix * vec4(0, 0, 1, 0)).xyz);

    int n = 0;
    vec3 v0 = positionAttribute.xyz;
    // compute derivative in direction t1
    vec4 projectedView = normalize(vec4(viewDirection -
        (dot(viewDirection, normalAttribute.xyz) * normalAttribute.xyz), 0));

    vec3 t1 = projectedView.xyz;
    vec3 t2 = cross(normals[gl_VertexID].xyz, t1);
    float v0_dot_t2 = dot(v0, t2);


    // the following code is taken from the princton real time suggestive contours app: rtsc
    
	uint numNeighbors = neighbors[gl_VertexID][max_expected_neighbors - 1]/2;
	for(int i = 0; i < numNeighbors; i++)
    {
        ivec2 index = ivec2(neighbors[gl_VertexID][2 * i],neighbors[gl_VertexID][2 * i + 1]);
        vec3 v1 = vertices[index.x].xyz;
        vec3 v2 = vertices[index.y].xyz;

        // Find the point p on the segment between v1 and v2 such that
        // its vector from v0 is along t1, i.e. perpendicular to t2.
        // Linear combination: p = w1*v1 + w2*v2, where w2 = 1-w1
        float v1_dot_t2 = dot(v1, t2);
        float v2_dot_t2 = dot(v2, t2);
        float w1 = (v2_dot_t2 - v0_dot_t2) / (v2_dot_t2 - v1_dot_t2);

        // If w1 is not in [0..1) then we're not interested.
        // Incidentally, the computation of w1 can result in infinity,
        // but the comparison should do the right thing...
        if (w1 < 0.0f || w1 > 1.0f)
           continue;

        // Construct the opposite point
        float w2 = 1.0f - w1;
        vec3 p = w1 * v1 + w2 * v2;

        // And interpolate to find the view-dependent curvature at that point
        float interp_radial_curv = w1 * curvature[index.x].w + w2 * curvature[index.y].w;

        // Finally, take the *projected* view-dependent curvature derivative
        float proj_dist = dot((p - v0), t1);
        result += (interp_radial_curv - curvature[gl_VertexID].w) / proj_dist;
        n++;

        // To save time, quit as soon as we have two estimates
        // (that's all we're going to get, anyway)
        if (n == 2) 
	{
            result *= 0.5f;
            break;
        }
    }

    return result;
}

void main()
{
	float radialCurvature = computeRadialCurvatureDerivative();

	derivative[gl_VertexID] = radialCurvature;
	passRadialCurvDeriv =  vec4(radialCurvature);
	//passRadialCurvDeriv =  curvature[gl_VertexID];
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * positionAttribute;

}
