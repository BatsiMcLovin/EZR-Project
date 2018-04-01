#version 450

out vec4 passCurv_c;
out vec4 passCurv;
out vec4 passPosition_c;
out vec4 passNormal_c;
out vec4 passNormal_w;
out vec4 passPosition_w;
out float passDeriv;

uniform mat4 modelMatrix = mat4(1.0f);
uniform mat4 viewMatrix = mat4(1.0f);
uniform mat4 projectionMatrix = mat4(1.0f);

uniform int snapGradientAmount = 0;

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

void main()
{
    passPosition_w = positionAttribute;
    passDeriv = derivative[gl_VertexID];
	passCurv = curvature[gl_VertexID];
    passCurv_c = ( transpose( inverse( viewMatrix * modelMatrix ) ) * passCurv);
    
        if(snapGradientAmount < 0.0)
    {
    	passCurv_c = round(passCurv_c * snapGradientAmount) / float(snapGradientAmount);
    }
    
    passNormal_c = ( transpose( inverse( viewMatrix * modelMatrix ) ) * normalAttribute );
    passNormal_w = normalAttribute;
    passPosition_c = viewMatrix * modelMatrix * positionAttribute;
    gl_Position = projectionMatrix * passPosition_c;
}
