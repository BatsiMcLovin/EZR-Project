#version 330

in vec2 passUV;

uniform sampler2DArray tex;
uniform int layer = 0;

out vec4 fragcolor;

void main() {

fragcolor = texture(tex, vec3(passUV, layer));

}