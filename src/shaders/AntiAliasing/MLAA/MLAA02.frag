#version 330 core

uniform sampler2D tex;
uniform sampler2D edges;

in vec2 passUV;

out vec4 fragcolor;

void main()
{
    vec2 size = textureSize(edges, 0);

    vec2 ps = 1.0 / size;

    vec4 topLeft = texture(edges, passUV);
    float bottom = texture(edges, passUV + (vec2(0,-1) * ps)).g;
    float right = texture(edges, passUV + (vec2(1,0) * ps)).a;

    vec4 a = vec4(topLeft.r, bottom, topLeft.b, right);
    vec4 w = abs(a * a * a);

    float sum = dot(w, vec4(1.0));


    if(sum > 0.0001)
    {
        vec4 color = vec4(0.0);
        vec4 tc = vec4(0.0, a.r, 0.0, -a.g) * ps.xyxy + passUV.xyxy;

        color += texture(tex, tc.xy) * w.r;
        color += texture(tex, tc.zw) * w.g;

        tc = vec4(-a.b, 0.0, a.a, 0.0) * ps.xyxy + passUV.xyxy;

        color += texture(tex, tc.xy) * w.b;
        color += texture(tex, tc.zw) * w.a;

        fragcolor = clamp(color/sum, 0.0, 1.0);
    }
    else
        fragcolor = texture(tex, passUV);
}