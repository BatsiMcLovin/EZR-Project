#version 450
layout(early_fragment_tests) in;

in vec4 color;
out vec4 fragcolor;

void main()
{

fragcolor = color;

}
