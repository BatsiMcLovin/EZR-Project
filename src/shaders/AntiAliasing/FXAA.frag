#version 330 core

in vec2 passUV;

uniform sampler2D tex;
uniform vec2 resolution;


void main() {
vec2 texelSize = 1.0 / resolution;

vec4 colorM = texture(tex, passUV) * 0.5;
vec4 colorN = texture(tex, passUV + texelSize) * 0.125;
vec4 colorS = texture(tex, passUV - texelSize) * 0.125;

texelSize.x = -texelSize.x;

vec4 colorE = texture(tex, passUV + texelSize) * 0.125;
vec4 colorW = texture(tex, passUV - texelSize) * 0.125;

gl_FragColor = (colorM + colorN + colorS + colorE + colorW);

}