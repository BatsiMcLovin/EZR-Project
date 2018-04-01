#version 330 core

uniform sampler2D texA;
uniform sampler2D texB;

in vec2 passUV;

void main()
{
vec4 result = vec4(0);
vec4 toonResult = result = texture(texA, passUV);

    vec4[9] values;
    values[4] = texture(texB, passUV);

    if(length(values[4]) < 0.0001)
    {
    gl_FragColor = toonResult;
    return;
    }

    values[0] = textureOffset(texB, passUV, ivec2(-1,  1));
    values[1] = textureOffset(texB, passUV, ivec2(-1,  0));
    values[2] = textureOffset(texB, passUV, ivec2(-1, -1));

    values[3] = textureOffset(texB, passUV, ivec2( 0,  1));
    values[5] = textureOffset(texB, passUV, ivec2( 0, -1));
    
    values[6] = textureOffset(texB, passUV, ivec2( 1,  1));
    values[7] = textureOffset(texB, passUV, ivec2( 1,  0));
    values[8] = textureOffset(texB, passUV, ivec2( 1, -1));



    int blackCount = 0;
    for(int i = 0; i < 9; i++){
       if (length(values[i].xyz) < 0.0001)
            blackCount++;
    }


    if(blackCount >= 5)
        result = toonResult;
    else
        result = values[4] + toonResult;

        gl_FragColor = result;
}
