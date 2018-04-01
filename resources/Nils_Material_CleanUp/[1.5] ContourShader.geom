#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 Vertex_geom[];
in vec4 Normal_geom[];

out float yCoord;
out vec4 passPosition;
out float contourParam;

uniform vec3 viewDirection;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

uniform float widthFactor = 1;

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

float parameterize(vec3 vertexNormal, vec3 edge, vec3 pos)
{
    float result;

    vec3 refPoint = vec3(-10,0, 0);

    // compute normal of the contour patch
    vec3 temp = normalize(cross(edge, vertexNormal));
    vec3 patchNormal = normalize(cross(temp, edge));

    // world coordinate axles are used as reference
    // rotate the normal so that it's x component is 0

    // test: do this only in xy-plane -> reference axis = y, rotation axis = z
    patchNormal.z = 0;
    normalize(patchNormal);
    vec3 referenceAxis = vec3(0,1,0);

    float angle = acos(dot(referenceAxis, patchNormal)) / 16.0;

    vec3 v = cross(patchNormal, referenceAxis);
    float s = length(v);
    float c = dot(patchNormal, referenceAxis);

    mat3 vX = mat3(0, -v.z, v.y,
        v.z, 0, -v.x,
        -v.y, v.x, 0);

        mat3 R = mat3(1) + vX + vX * vX * (1-c) / (s*s);
        R = inverse(transpose(R));

        vec3 rotPos = R * (pos - refPoint) + refPoint;

        result = rotPos.x - refPoint.x;
        return result;
    }

    vec4 getZeroCrossing(vec4 p1, vec4 p2, float r1, float r2)
    {
        float x = -r2 / (r1 - r2);
        vec4 p = x*p1 + (1 - x)*p2;
        p.w = 1;
        return p;
    }

    void drawQuad(vec4 p1, vec4 p2, vec4 n1, vec4 n2, float width)
    {
        passPosition = projectionMatrix * modelViewMatrix * (p1+width*n1);
        gl_Position = passPosition;
        yCoord = 1;
        EmitVertex();

        passPosition = projectionMatrix * modelViewMatrix * (p1 - 0*width*n1);
        gl_Position = passPosition;
        yCoord = 0;
        EmitVertex();

        passPosition = projectionMatrix * modelViewMatrix * (p2 + width*n2);
        gl_Position = passPosition;
        yCoord = 1;
        EmitVertex();

        passPosition = projectionMatrix * modelViewMatrix * (p2 - 0*width*n2);
        gl_Position = passPosition;
        yCoord = 0;
        EmitVertex();

        EndPrimitive();
    }

    void generateContours()
    {
        // get the vertices for GL_TRIANGLES_ADJACENCY
        vec4 v0 = Vertex_geom[0];
        vec4 v1 = Vertex_geom[1];
        vec4 v2 = Vertex_geom[2];

        // compute triangle normals
        vec4 n0 = Normal_geom[0];
        vec4 n1 = Normal_geom[1];
        vec4 n2 = Normal_geom[2];

        float dot0 = dot(viewDirection, n0.xyz);
        float dot1 = dot(viewDirection, n1.xyz);
        float dot2 = dot(viewDirection, n2.xyz);

        bool b01 = dot0*dot1 < 0;
        bool b12 = dot1*dot2 < 0;
        bool b20 = dot2*dot0 < 0;

        float width = widthFactor;

        if (b01 && b12)
        {
            vec4 p1 = getZeroCrossing(v0, v1, dot0, dot1);
            vec4 p2 = getZeroCrossing(v1, v2, dot1, dot2);
            vec4 Nor1 = getZeroCrossing(n0, n1, dot0, dot1);
            Nor1.w = 0;
            Nor1.xyz = normalize(Nor1.xyz);
            vec4 Nor2 = getZeroCrossing(n1, n2, dot1, dot2);
            Nor2.w = 0;
            Nor2.xyz = normalize(Nor2.xyz);
            drawQuad(p1, p2, Nor1, Nor2, width);
        }

        if (b01 && b20)
        {
            vec4 p1 = getZeroCrossing(v0, v1, dot0, dot1);
            vec4 p2 = getZeroCrossing(v2, v0, dot2, dot0);
            vec4 Nor1 = getZeroCrossing(n0, n1, dot0, dot1);
            Nor1.w = 0;
            Nor1.xyz = normalize(Nor1.xyz);
            vec4 Nor2 = getZeroCrossing(n2, n0, dot2, dot0);
            Nor2.w = 0;
            Nor2.xyz = normalize(Nor2.xyz);
            drawQuad(p1, p2, Nor1, Nor2, width);
        }

        if (b12 && b20)
        {
            vec4 p1 = getZeroCrossing(v1, v2, dot1, dot2);
            vec4 p2 = getZeroCrossing(v2, v0, dot2, dot0);
            vec4 Nor1 = getZeroCrossing(n1, n2, dot1, dot2);
            Nor1.w = 0;
            Nor1.xyz = normalize(Nor1.xyz);
            vec4 Nor2 = getZeroCrossing(n2, n0, dot2, dot0);
            Nor2.w = 0;
            Nor2.xyz = normalize(Nor2.xyz);
            drawQuad(p1, p2, Nor1, Nor2, width);
        }
    }

    void main()
    {
        generateContours();
    }
