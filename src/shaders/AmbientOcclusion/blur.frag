#version 330 core

#define kernelwidth 3
#define kernelsize kernelwidth*kernelwidth

uniform sampler2D aoTex;
uniform sampler2D depthTex;

in vec2 passUV;

float kernel[kernelsize] = float[](

   0.014614,   0.107982,   0.014614,
   0.107982,   0.797885,   0.107982,
   0.014614,   0.107982,   0.014614

);


vec4 simpleBlur()
{

float color = 0.0;
for(int i = 0; i < kernelwidth; i++)
{

    for(int j = 0; j < kernelwidth; j++)
    {
    float weight = kernel[i * kernelwidth + j];
    color += weight * texelFetch(aoTex, ivec2(gl_FragCoord.xy) + ivec2(i - 1, j - 1),0).r;
    }

}

return vec4(color);
}

void main()
{

if(texture(depthTex, passUV).r > -0.1)
    gl_FragColor = simpleBlur();
}