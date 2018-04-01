#version 330

uniform sampler2D texA;
uniform sampler2D texB;

in vec2 passUV;
out vec4 fragcolor;

void main()
{
    fragcolor = texture(texA, passUV) * texture(texB, passUV);
}