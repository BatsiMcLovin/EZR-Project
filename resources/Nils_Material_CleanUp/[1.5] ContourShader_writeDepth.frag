#version 450
layout(early_fragment_tests) in;

in vec4 passPosition;
in float yCoord;
out vec4 fragColor;
out vec4 fragDepth;

uniform vec4 contourColor = vec4(1);

void main()
{
	fragColor = contourColor;
	ivec2 coord = ivec2(gl_FragCoord.xy);

	// depth
	float far=gl_DepthRange.far; float near=gl_DepthRange.near;
	float ndc_depth = passPosition.z / passPosition.w;
	float depth = (((far-near) * ndc_depth) + near + far) / 2.0;
	float depthVal = 0.5 * ndc_depth + 0.5;
	
	fragDepth = vec4(depthVal);
}
