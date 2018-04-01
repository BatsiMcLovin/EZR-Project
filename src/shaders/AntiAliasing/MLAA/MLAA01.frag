#version 330 core

uniform sampler2D tex;
uniform sampler2D lut;

in vec2 passUV;

out vec4 fragcolor;

uniform int max_search = 8;

float search(sampler2D edgeTex, vec2 pixelSize, vec2 uv, vec2 dir)
{
    vec2 stepSize = dir * pixelSize;
    vec2 tc = uv + stepSize * 1.5;

    stepSize = stepSize * 2.0;

    float edge = 0.0;
    float incrementDir = dot(vec2(1.0), dir);
    vec2 axis = abs(dir);

    int i = 0;

    for( i = 0; i < max_search; i++)
    {
        edge = dot(texture(edgeTex, tc).yx * axis, vec2(1.0));
        if(edge < 0.9)
        break;

        tc = tc + stepSize;
    }

   return min(2.0 * (i + edge), 2.0 * max_search) * incrementDir;
}


vec2 area(sampler2D lutTex, vec2 sampledEdgePos, float edge01, float edge02)
{

/**/
    vec2 lutSize = textureSize(lutTex, 0);

    vec2 subTexTC = vec2(edge01, edge02) * 0.8;
    vec2 interSubTC = sampledEdgePos / (max_search * 5.0);

    interSubTC = clamp(interSubTC, 0.0, 0.2);

    vec2 tc = interSubTC + subTexTC;

    vec2 rest = tc * lutSize;
    vec2 rest02 = rest;
    rest = round(rest);

    tc = rest / lutSize;

    ivec2 tci = ivec2(rest.x, lutSize.y - rest.y);

    return texelFetch(lutTex, tci, 0).rg;
    //return texture(lutTex, vec2(tc.x, 1.0 - tc.y)).rg;

}


vec4 blendWeight(sampler2D edgeTex, sampler2D lutTex, vec2 uv, vec2 pixelSize)
{

    ivec2 tci = ivec2(passUV * textureSize(tex, 0));

    vec2 edge = texelFetch(tex, tci, 0).rg;
    vec4 weights = vec4(0.0);

    if(edge.y > 0.0)
    {
        float edgeLeft = search(edgeTex, pixelSize, uv, vec2(-1,0));
        float edgeRight = search(edgeTex, pixelSize, uv, vec2(1,0));

        float edge01 = texture(edgeTex, uv + vec2(edgeLeft, 0.25) * pixelSize).r;
        float edge02 = texture(edgeTex, uv + vec2(edgeRight + 1.0, 0.25) * pixelSize).r;

        weights.rg = area(lutTex, abs(vec2(edgeLeft, edgeRight)), edge01, edge02);
    }

    if(edge.x > 0.0)
    {
        float edgeUp = search(edgeTex, pixelSize, uv, vec2(0,1));
        float edgeDown = search(edgeTex, pixelSize, uv, vec2(0,-1));

        float edge01 = texture(edgeTex, uv + vec2(-0.25, edgeUp) * pixelSize).g;
        float edge02 = texture(edgeTex, uv + vec2(-0.25, edgeDown - 1.0) * pixelSize).g;

        weights.ba = area(lutTex, abs(vec2(edgeUp, edgeDown)), edge01, edge02);
    }

        return clamp(weights, vec4(0.0), vec4(1.0));

}

void main()
{

    vec2 texelSize = 1.0 / textureSize(tex, 0);

    fragcolor = blendWeight(tex, lut, passUV, texelSize);
}