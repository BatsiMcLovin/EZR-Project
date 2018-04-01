#version 430

layout (binding = 0, offset = 0) uniform atomic_uint aCounter;

uniform float numPixels = 1.0f;

out vec4 fragcolor;


vec3 lerpColors(vec3 c0, vec3 c1, vec3 c2, vec3 c3, float t, bool doClamp)
{
    t = doClamp ? clamp(t, 0.0f, 1.0f) : 1 - abs(mod(t, 2) - 1);
    t *= 3.0f;

    if(t < 1.0f)
        return mix(c0, c1, t);
    if(t < 2.0f)
        return mix(c1, c2, t - 1.0f);
    else
        return mix(c2, c3, t - 2.0f );
}

void main()
{
    uint count = atomicCounterIncrement(aCounter);

    fragcolor = vec4(lerpColors(vec3(1,0,0), vec3(1,1,0), vec3(0,1,0), vec3(0,1,1), count / numPixels, false), 1) ;
}