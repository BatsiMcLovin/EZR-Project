#version 330 core

uniform sampler2D colorTexture;
in vec2 passUV;

void main()
{
    gl_FragColor = texture(colorTexture, passUV);
}