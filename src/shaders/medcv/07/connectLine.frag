#version 450

in vec2 passUV;

uniform sampler2D licTexture;
uniform sampler2D curvatureTexture;

uniform float smoothFactor = 0.0f;
uniform vec2 screenSize;

uniform float smoothingLookUp = 2.0f;

out vec4 fragcolor;


vec2 rotate2D(vec2 vector, float angle)
{
	mat2 rotMatrix = mat2( 	cos(angle), -sin(angle), 
							sin(angle), cos(angle));
	
	return rotMatrix * vector;

}

void main() {

vec4 thisPixel = texture(licTexture, passUV);

if(thisPixel.w <= 0.0f)
{
	fragcolor = thisPixel;
	return;
}


	fragcolor = textureLod(licTexture, passUV, 0);
	return;

vec4 gradientDir = texelFetch(curvatureTexture, ivec2(gl_FragCoord), 0);

vec2 lookUpDir = rotate2D(normalize(gradientDir.xy), 1.57079632679);

fragcolor = thisPixel + textureLod(licTexture, passUV + ((lookUpDir * smoothingLookUp)/ screenSize), smoothFactor);
float factor = fragcolor.w * 0.5;

if(factor == 0)
	factor = 1;

//fragcolor = textureLod(licTexture, passUV, (1 - factor) * 5);
//fragcolor = vec4(factor);
}
