#version 330 core

uniform sampler2D colorTexture;
in vec2 passUV;
in vec3 passNormal;

void main(){


gl_FragColor = texture(colorTexture, passUV);

}