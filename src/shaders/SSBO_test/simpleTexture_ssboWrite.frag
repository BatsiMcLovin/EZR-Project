#version 450

in vec2 passUV;
in vec4 ssboContents;

uniform sampler2D tex;

out vec4 fragcolor;

void main() {

//fragcolor = texture(tex, passUV) * ssboContents;
fragcolor = ssboContents;
}
