#version 330 core

in vec2 passUV;

uniform sampler2D tex;

out vec4 fragcolor;

void main() {

fragcolor = texture(tex, passUV);

}
