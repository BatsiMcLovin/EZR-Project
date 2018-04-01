#version 450

in vec2 passUV;

uniform sampler2D tex;

out vec4 fragcolor;

void main() {

fragcolor = texture(tex, passUV);
//fragcolor = vec4(passUV, 0.0f, 1.0f);
}
