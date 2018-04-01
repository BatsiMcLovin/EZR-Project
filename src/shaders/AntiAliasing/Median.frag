#version 330 core

in vec2 passUV;

uniform sampler2D tex;
uniform vec2 resolution;


void main() {
vec2 texelSize = 1.0 / resolution;

float m = 0.1;
float r = (1 - m) / 8.0;

vec4 colorM = texture(tex, passUV) * m;
vec4 colorNE = texture(tex, passUV + texelSize) * r;
vec4 colorSW = texture(tex, passUV - texelSize) * r;

texelSize.x = -texelSize.x;

vec4 colorNW = texture(tex, passUV + texelSize) * r;
vec4 colorSE = texture(tex, passUV - texelSize) * r;

vec2 texelSize_x = vec2(texelSize.x, 0.0);
vec2 texelSize_y = vec2(0.0, texelSize.y);

vec4 colorN = texture(tex, passUV + texelSize_y) * r;
vec4 colorS = texture(tex, passUV - texelSize_y) * r;

vec4 colorE = texture(tex, passUV + texelSize_x) * r;
vec4 colorW = texture(tex, passUV - texelSize_x) * r;


gl_FragColor = (colorM + colorN + colorS + colorE + colorW + colorNE + colorNW + colorSE + colorSW);

}