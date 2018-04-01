#version 330 core

uniform sampler2D tex;

in vec2 passUV;


out vec4 fragcolor;

float threshold = 0.1;



float toGrey(vec3 color)
{
return dot(color, vec3(0.2126, 0.7152, 0.0722));
}


void main() {

vec2 size = textureSize(tex, 0);

vec2 resolution = 1.0/size;

ivec2 tci = ivec2(passUV * size);

float center = toGrey(texelFetch(tex, tci, 0).xyz);
float left = toGrey(texelFetch(tex, tci + ivec2(-1.0, 0.0), 0).xyz);
float top = toGrey(texelFetch(tex, tci + ivec2(0.0, 1.0), 0).xyz);

vec2 delta = abs(vec2(center) - vec2(left,top));

vec2 edge = step(vec2(threshold), delta);

fragcolor = vec4(edge, 0.0, 1.0);
}