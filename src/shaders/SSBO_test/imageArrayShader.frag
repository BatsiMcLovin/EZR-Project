#version 450

uniform sampler2DArray texAvery;

in vec2 passUV;

out vec4 fragColor;

uniform int counter = 0;


float penis(image2D kack)
{

return 1;
}

void main()
{
//    vec2 size = vec2(imageSize(texAvery).xy);
    vec2 size = vec2(1024);

    fragColor = texelFetch(texAvery, ivec3(size * passUV, counter), 0);

}
