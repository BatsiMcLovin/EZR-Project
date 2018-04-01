#version 330 core

in vec2 passUV;

uniform sampler2D tex;
uniform sampler2D depthTex;

uniform vec2 resolution;


vec2 principalDir(vec2 texelSize)
{

vec2 dirs[8];
float grad[8];

dirs[0] = vec2(texelSize.x, 0.0);
dirs[1] = vec2(0.0, texelSize.y);
dirs[2] = texelSize;
dirs[3] = vec2(texelSize.x, -texelSize.y);

dirs[4] = -vec2(texelSize.x, 0.0);
dirs[5] = -vec2(0.0, texelSize.y);
dirs[6] = -texelSize;
dirs[7] = -vec2(texelSize.x, -texelSize.y);


float min = 1.05;
int min_i = -1;

for(int i = 0; i < 8; i++)
{
    grad[i] = texture(depthTex, passUV + dirs[i]).x / texture(depthTex, passUV).x;

    if(grad[i] > min)
    {
    min = grad[i];
    min_i = i;
    }
}

//return vec2(0.0, 1.0);

if(min_i >= 0)
    return dirs[min_i];
else
    return vec2(0.0, 0.0);
}

void main() {
vec2 texelSize = 1.0 / resolution;

vec2 dir = principalDir(texelSize);


float r = 0.7;
gl_FragColor = texture(tex, passUV) * r + texture(tex, passUV + dir) * (1-r);

/*
if(length(dir) > 0.0)
    gl_FragColor = (texture(tex, passUV) * 0.75) + (texture(tex, passUV + dir) * 0.25);
else
    gl_FragColor = texture(depthTex, passUV);
*/
}