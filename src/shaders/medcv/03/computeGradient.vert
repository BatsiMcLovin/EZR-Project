#version 450
#define max_expected_neighbors 40

layout(location = 0) in vec4 positionAttribute;
layout (location = 2) in vec4 normalAttribute;

uniform mat4 modelMatrix = mat4(1.0f);
uniform mat4 viewMatrix = mat4(1.0f);
uniform mat4 projectionMatrix = mat4(1.0f);

out vec4 outDebug;
out vec4 outposition;

float epsilon = 0.000001;
vec3 viewDirection = vec3(0, 0, 1);
uniform float sc_Scalar = 1.0;

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


float getVoronoiArea(ivec2 index)
{
    float voronoiArea = 0.0f;

    // get vertices and their normals
    vec3 p1 = vertices[gl_VertexID].xyz;
    vec3 n1 = normals[gl_VertexID].xyz;
    vec3 p2 = vertices[index.x].xyz;
    vec3 n2 = normals[index.x].xyz;
    vec3 p3 = vertices[index.y].xyz;
    vec3 n3 = normals[index.y].xyz;

    // compute all edges of the current triangle
    vec3 v1 = p2-p1;
    vec3 v2 = p3-p1;
    vec3 v3 = p3-p2;

    // compute face normal and triangle area
    vec3 faceNormal = cross(v1, v2);
    float doubleTriangleArea = length(faceNormal);

    // check triangles and get voronoi area
    int obtuseCorner = -1;
    if (dot(v1,v2) < 0) {
        obtuseCorner = 0;
    }

    else if (dot(-v1, v3) < 0){
        obtuseCorner = 1;
    }

    else if (dot(-v2, -v3) < 0){
        obtuseCorner = 2;
    }


    if(obtuseCorner > -1) {
        voronoiArea = doubleTriangleArea/2.0;
        // P is obtuse corner:
      if (obtuseCorner == 0)
        voronoiArea /= 2.0f;
        else
        voronoiArea /= 4.0f;
    }

    else {
        float cot_p2 = dot(v3,-v1)/length(cross(v3,-v1));
        float cot_p3 = dot(-v3,-v2)/length(cross(-v3,-v2));

        // voronoiArea calculation according to Meyer et al: Discrete Differential-Geometry Operators for Triangulated 2-Manifolds
        voronoiArea = ( (dot(v2,v2)*cot_p2) +(dot(v1,v1)*cot_p3) )/8.0;
    }

  return voronoiArea;
}

vec3[2] orthogonalTriangleBasis(vec3 normal)
{
    vec3[2] basis;
    
	basis[0] = cross(normal, vec3(1));

	if(length(basis[0]) < epsilon)
	{	
		basis[0]=vec3(1,-1,0);
	}

	basis[0] = normalize(basis[0]);
	basis[1]=normalize(cross(basis[0],normal));

	return basis;
}

vec3 getScalar(ivec2 index)
{
    vec3 scalar;
    vec3 n1 = normalAttribute.xyz;
    vec3 n2 = normals[index.x].xyz;
    vec3 n3 = normals[index.y].xyz;
    n1 = normalize(n1);
    n2 = normalize(n2);
    n3 = normalize(n3);
    scalar.x = dot(viewDirection, n1);
    scalar.y = dot(viewDirection, n2);
    scalar.z = dot(viewDirection, n3);
    return scalar;
}

float getSignedTriangleArea2D(vec2 p2,vec2 p3)
{
    return (p2.x*p3.y-p3.x*p2.y);
}

vec3 rotateVector(vec3 inputVec,vec3 rotationAxis, float angle)
{
    float c=cos(angle);
    float s=sin(angle);
    float x=rotationAxis.x;
    float y=rotationAxis.y;
    float z=rotationAxis.z;
    mat3 Rot=mat3(	c+x*x*(1-c),   y*x*(1-c)+z*s,  z*x*(1-c)-y*s,
					x*y*(1-c)-z*s, c+y*y*(1-c),    z*y*(1-c)+x*s,
					x*z*(1-c)+y*s, y*z*(1-c)-x*s,  c+z*z*(1-c));
    return Rot*inputVec;
}

vec3 faceGradient(ivec2 index)
{
    vec3 v1 = vertices[index.x].xyz-positionAttribute.xyz;
    vec3 v2 = vertices[index.y].xyz-positionAttribute.xyz;
    vec3 faceNormal = normalize(cross(v1,v2));

	// FUNCTION CALL   
    vec3[2] triangleBasis = orthogonalTriangleBasis(faceNormal);
    
    vec2 p2 = vec2(dot(triangleBasis[0],v1), dot(triangleBasis[1],v1));
    vec2 p3 = vec2(dot(triangleBasis[0],v2), dot(triangleBasis[1],v2));

	// FUNCTION CALL    
	float triangleArea = getSignedTriangleArea2D(p2,p3);
    
	// FUNCTION CALL
    vec3 scalar = getScalar(index);
	vec3 faceGrad = (scalar.y-scalar.x)*rotateVector(-v2,faceNormal,acos(0))/(2*triangleArea)+(scalar.z-scalar.x)*rotateVector(v1,faceNormal,acos(0))/(2*triangleArea);
    return faceGrad;
}

vec3 lightGradient()
{
    vec3 resGradient=vec3(0);
    
    float voroRes=0;
	uint numNeiHalf = neighbors[gl_VertexID][max_expected_neighbors - 1]/2;

	if(numNeiHalf < 1)
	{
		return vec3(0);
	}

    for(int i = 0; i < numNeiHalf; i++)
	{

		int index0 = neighbors[gl_VertexID][2 * i];
		int index1 = neighbors[gl_VertexID][2 * i + 1];
        ivec2 index = ivec2(index0, index1);
		// FUNCTION CALL        
		vec3 faceGrad = faceGradient(index);  
        vec3 v1 = vertices[index.x].xyz-positionAttribute.xyz;
        vec3 v2 = vertices[index.y].xyz-positionAttribute.xyz;
        vec3 faceNormal = normalize(cross(v1,v2));
        vec3 vertexNormal = normalAttribute.xyz;
        if(dot(faceNormal,vertexNormal)<0)
			faceNormal=-faceNormal;
        vertexNormal = normalize(vertexNormal);
        vec3 rotVector = normalize(cross(faceNormal,vertexNormal));
        if(length(faceNormal-vertexNormal)<epsilon)
			{
				rotVector = vertexNormal;	
			}
        float angle = acos(clamp(dot(faceNormal,vertexNormal),-1,1));
        faceGrad = rotateVector(faceGrad, rotVector,angle);
		// FUNCTION CALL
        float voroArea = getVoronoiArea(index);
        resGradient += faceGrad * voroArea;
        voroRes += voroArea;
    }

	return resGradient/voroRes;
}


vec3 demNormals()
{
	uint numNeiHalf = neighbors[gl_VertexID][max_expected_neighbors - 1]/2;
	vec3 normalKing;

	for (int i = 0; i < numNeiHalf; i++ )
	{
		vec3 n0 = normals[neighbors[gl_VertexID][0]].xyz;
		vec3 n1 = normals[neighbors[gl_VertexID][1]].xyz;
		normalKing = normalize(cross(n0, n1));
	}

	return normalize(normalKing);
}

void main() {

	viewDirection = normalize((viewMatrix * vec4(0, 0, 1, 0)).xyz);

    vec3 resGradient = lightGradient(); // FUNCTION CALL
    outposition.xyz = resGradient;
    gl_Position =  projectionMatrix * viewMatrix * modelMatrix * positionAttribute;

    // radial curvature in direction of the projected view vector
    outposition.w = sc_Scalar * dot(resGradient, viewDirection - dot(normalAttribute.xyz, viewDirection) * normalAttribute.xyz);


	curvature[gl_VertexID] = outposition;

	outDebug = vec4(abs(viewDirection), 1);
	outDebug = abs(vec4(outposition.w));
}
