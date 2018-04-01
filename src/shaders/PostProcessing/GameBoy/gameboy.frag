#version 330

uniform sampler2D tex;

uniform vec2 screenSize = vec2(800, 600);
uniform bool gameboy_use_green = false;
uniform int steps = 8;

out vec4 fragcolor;

in vec2 passUV;

vec3 rgb2hsv2(vec3 rgbColor)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(rgbColor.bg, K.wz), vec4(rgbColor.gb, K.xy), step(rgbColor.b, rgbColor.g));
    vec4 q = mix(vec4(p.xyw, rgbColor.r), vec4(rgbColor.r, p.yzx), step(p.x, rgbColor.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 hsvColor)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(hsvColor.xxx + K.xyz) * 6.0 - K.www);
    return hsvColor.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), hsvColor.y);
}

void main()
{
    vec2 offset = mod(gl_FragCoord.y, 2) != 0 ? passUV : (screenSize * passUV + vec2(5, 0)) / screenSize;
    vec4 rgbColor = texture(tex, offset);


    if(gameboy_use_green)
    {
        float gray = dot(vec3(0.21, 0.72, 0.07), rgbColor.xyz);

        gray = floor(gray * steps) / (steps - 1.0f);
        rgbColor = vec4(0.7, 0.77, 0.01, 1.0f) * gray;
    }
    else
    {
        float stepLength = floor(length(rgbColor.xyz) * steps) / (steps - 1.0f);
        vec3 hsvColor = rgb2hsv2(rgbColor.xyz);

        hsvColor.yz = floor((hsvColor.yz) * steps) / (steps - 1.0f);
        int steps2 = steps * 2;
        hsvColor.x = round((hsvColor.x) * steps2 ) / (steps2 - 1.0f);


        rgbColor.xyz = hsv2rgb(hsvColor);
        //rgbColor.xyz = abs(rgbColor.xyz - hsv2rgb(hsvColor));
        //rgbColor.xyz = normalize(rgbColor.xyz) * stepLength;
    }

    float vertical_pattern = (sin(passUV.y * 200.0f) * 0.5 + 0.5f) > 0.1f ? 1.0 : 0.0;
    fragcolor = rgbColor - (vertical_pattern * 0.0125f);
}
