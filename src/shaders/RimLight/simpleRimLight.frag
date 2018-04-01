#version 330

uniform sampler2D normalTex;
uniform sampler2D colorTex;
uniform sampler2D rimlightTex;

in vec2 passUV;

void main() {

vec3 lookDir = vec3(0.0, 0.0, -1.0f);

int level = 4;

vec3 normal = textureLod(normalTex, passUV, level).xyz;
vec4 color = textureLod(colorTex, passUV, level);
float lightness = textureLod(rimlightTex, passUV, level).x;

float rimlight = (dot(lookDir, normalize(normal)) + 1) * 0.5f ;

//rimlight = (rimlight * rimlight);

gl_FragColor = color + vec4(rimlight, rimlight, rimlight, 1) * lightness;

}