//FRAGMENT SHADER
#version 330 core

layout(location = 0) out vec4 reflectionsOutput;

uniform sampler2D depthTex;
uniform sampler2D normalTex;
uniform sampler2D colorTex;
uniform sampler2D positionTex;
uniform sampler2D reflectivityTex;

uniform int width;
uniform int height;
uniform float far;
uniform float near;

in vec2 passUV;

uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);


vec4 doSSR(vec3 positionSS, vec3 reflectedViewDirectionSS){
    vec4 reflectedColor = vec4(0.0);

    // raymarching along the reflected view direction
    float stepSize = 1.0/height;
    vec3 reflectedVectorSS = reflectedViewDirectionSS;
    reflectedVectorSS = normalize(reflectedVectorSS) * stepSize;

    vec3 lastDepth = positionSS + reflectedVectorSS;
    vec3 sampleDepth = lastDepth + reflectedVectorSS;

    float step = 12;
    int count = 0;

    while(count < 20){
    
        if(sampleDepth.x < 0.0 || sampleDepth.x > 1.0 ||
           sampleDepth.y < 0.0 || sampleDepth.y > 1.0 ||
           sampleDepth.z < 0.0 || sampleDepth.z > 1.0){
            break;
        }

        vec2 samplePos = sampleDepth.xy;
        float sampledDepth = (2.0 * near) / (far + near - texture(depthTex, sampleDepth.xy).x * (far - near));
        float currentDepth = (2.0 * near) / (far + near - sampleDepth.z * (far - near));

        if(currentDepth < sampledDepth)
        {
            lastDepth = sampleDepth;
            sampleDepth = lastDepth + reflectedVectorSS * step;
        }
        else{
            if(currentDepth > sampledDepth)
            {
                for(float i=0; i < step; i += 1.0)// * factor)
                {
                    lastDepth = sampleDepth;
                    sampleDepth = lastDepth - reflectedVectorSS * 0.05 * i;
                }

            }
            if(abs(currentDepth - sampledDepth) < 0.02)
            {
                    reflectedColor = texture(colorTex, samplePos);
            }
                break;
            }

        count++;
    }
    // Fading to screen edges
    vec2 fadeToScreenEdge = vec2(1.0);
        fadeToScreenEdge.x = distance(lastDepth.x , 1.0);
        fadeToScreenEdge.x *= distance(lastDepth.x, 0.0) * 4.0;
        fadeToScreenEdge.y = distance(lastDepth.y, 1.0);
        fadeToScreenEdge.y *= distance(lastDepth.y, 0.0) * 4.0;

    return (reflectedColor) * fadeToScreenEdge.x * fadeToScreenEdge.y;
}

void main(void)
{
    float reflectance = texture(reflectivityTex, passUV).a;

    if(reflectance > 0){
        vec3 positionVS = (viewMatrix * texture(positionTex, passUV)).xyz;
        vec3 normalVS = (transpose(inverse(viewMatrix)) * texture(normalTex, passUV)).xyz;

        vec3 viewDirection = normalize(positionVS);
        vec3 reflectedViewDirectionVS = normalize(reflect(viewDirection, normalVS));
        normalVS = normalize(normalVS);

        //transformation in Screen Space

            vec4 positionCS = projectionMatrix * vec4(positionVS, 1.0);
            vec3 positionSS = 0.5 * (positionCS.xyz/positionCS.w) + 0.5;

            reflectedViewDirectionVS += positionVS;
            vec4 reflectedViewDirectionCS = projectionMatrix * vec4(reflectedViewDirectionVS, 1.0);
            vec3 reflectedViewDirectionSS = 0.5 * (reflectedViewDirectionCS.xyz / reflectedViewDirectionCS.w) + 0.5;
            reflectedViewDirectionSS = normalize(reflectedViewDirectionSS - positionSS);

        reflectionsOutput = doSSR(positionSS, reflectedViewDirectionSS) * reflectance;
     }
     else
        reflectionsOutput = vec4(0);
}