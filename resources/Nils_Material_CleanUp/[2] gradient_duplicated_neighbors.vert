#version 450

layout(location = 0) in vec4 positionAttribute;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 passColor;
out vec4 outposition;
float epsilon=0.000001;
float PI=3.14159265359f;
uniform vec3 viewDirection;
uniform float sc_Scalar = 1.0;

// nei Information:
// nei[index].y=number of neighbors of index vertex
// nei[index].z=offset for neighbors (=offset)
// nei[offset].x=neighbor index

layout(std430) restrict buffer Nei_numNei
{
    uint nei_numNei[];
};

layout(std430) restrict buffer Nei_offset
{
    uint nei_offset[];
};

layout(std430) restrict buffer Nei_indices
{
    uint nei_indices[];
};

layout(std430) restrict buffer Nor
{
    vec3 normals[];
};

layout(std430) restrict buffer Vert
{
    vec3 vertices[];
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
    vec3[2] orthogonalTriangleBasis;
    if(normal.x!=normal.y || normal.x!=normal.z || normal.y!=normal.z)
		{
        orthogonalTriangleBasis[0]=normalize(vec3(normal.y-normal.z,normal.z-normal.x,normal.x-normal.y));
        orthogonalTriangleBasis[1]=normalize(cross(orthogonalTriangleBasis[0],normal));
    }

	else
		{
        orthogonalTriangleBasis[0]=normalize(vec3(1,-1,0));
        orthogonalTriangleBasis[1]=normalize(cross(orthogonalTriangleBasis[0],normal));
    }

	return orthogonalTriangleBasis;
}

vec3 getScalar(ivec2 index)
{
    vec3 getScalar;
    vec3 vd=-normalize(viewDirection);
    vec3 n1=normals[gl_VertexID].xyz;
    vec3 n2=normals[index.x].xyz;
    vec3 n3=normals[index.y].xyz;
    n1=normalize(n1);
    n2=normalize(n2);
    n3=normalize(n3);
    getScalar.x=dot(vd,n1);
    getScalar.y=dot(vd,n2);
    getScalar.z=dot(vd,n3);
    return getScalar;
}

float getTriangleArea(ivec2 index)
{
    float triangleArea;
    vec3 v1=vertices[index.x].xyz-positionAttribute.xyz;
    vec3 v2=vertices[index.y].xyz-positionAttribute.xyz;
    triangleArea=length(cross(v1,v2));
    return triangleArea/2.0f;
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
    vec3 faceGradient, allOne=vec3(1), faceNormal;
    vec3 v1=vertices[index.x].xyz-positionAttribute.xyz;
    vec3 v2=vertices[index.y].xyz-positionAttribute.xyz;
    faceNormal=normalize(cross(v1,v2));
    vec3[2] triangleBasis=orthogonalTriangleBasis(faceNormal);
    vec2 p1=vec2(0,0);
    vec2 p2=vec2(dot(triangleBasis[0],v1),dot(triangleBasis[1],v1));
    vec2 p3=vec2(dot(triangleBasis[0],v2),dot(triangleBasis[1],v2));
    vec3 px=vec3(p1.x,p2.x,p3.x);
    vec3 py=vec3(p1.y,p2.y,p3.y);
    mat3x2 M=transpose(mat2x3(cross(py,allOne),-cross(px,allOne)));
    float triangleArea=getSignedTriangleArea2D(p2,p3);
    vec3 scalar;
    scalar=getScalar(index);
    vec3 R=(scalar.y-scalar.x)*rotateVector(-v2,faceNormal,acos(0))/(2*triangleArea)+(scalar.z-scalar.x)*rotateVector(v1,faceNormal,acos(0))/(2*triangleArea);
    faceGradient=R;
    return faceGradient;
}

vec3 lightGradient()
{
    vec3 resGradient=vec3(0);
    vec3 FG;
    float voroRes=0;
    for(int i=0;i<nei_numNei[gl_VertexID]/2;i++)
	{
        uint off=nei_offset[gl_VertexID];
        ivec2 index=ivec2(nei_indices[off+2*i],nei_indices[off+2*i+1]);
        vec3 faceGradient=faceGradient(index);
        vec3 v1=vertices[index.x].xyz-positionAttribute.xyz;
        vec3 v2=vertices[index.y].xyz-positionAttribute.xyz;
        vec3 faceNormal=normalize(cross(v1,v2));
        vec3 vertexNormal=normals[gl_VertexID].xyz;
        if(dot(faceNormal,vertexNormal)<0)
			faceNormal=-faceNormal;
        vertexNormal=normalize(vertexNormal);
        vec3 rotVector=normalize(cross(faceNormal,vertexNormal));
        if(length(faceNormal-vertexNormal)<epsilon)
			rotVector=vertexNormal;
        float angle=acos(clamp(dot(faceNormal,vertexNormal),-1,1));
        faceGradient=rotateVector(faceGradient,rotVector,angle);
        float voroArea=getVoronoiArea(index);
        resGradient+=faceGradient*voroArea;
        voroRes+=voroArea;
    }

return resGradient/voroRes;
}

void main() {
    vec3 resGradient=lightGradient();
    vec3 vd=-normalize(viewDirection);
    outposition.xyz=resGradient;
    gl_Position =  projection * view * model * positionAttribute;

    // radial curvature in direction of the projected view vector
    outposition.w=sc_Scalar*dot(resGradient, vd-dot(normals[gl_VertexID].xyz,vd)*normals[gl_VertexID].xyz);

    // Debug ...
    passColor = outposition;

}
