#version 330

layout(location = 0) out vec4 sobelOutput;
layout(location = 1) out vec4 cellOutput;
layout(location = 2) out vec4 compositionOutput;

uniform sampler2D depthTex;
uniform sampler2D normalTex;
uniform sampler2D colorTex;
uniform sampler2D positionTex;
uniform sampler2D toonRamp;
uniform sampler2D outLineTex;
uniform sampler2D materialTex;
uniform sampler2D shadowTex;

uniform mat4 projectionMatrix = mat4(1.0);

uniform vec3 pointLight = vec3(10.0);
uniform float maxDist = 50.0f;

uniform float threshholdNormal;
uniform float threshholdDiffuse;
uniform float threshholdDepth;

uniform float shadow_border_threshold = 0.5f;

in vec2 passUV;

float minDepth = 0.0;


//GET MIN DEPTH VAL
float getMinDepth(float[9] values)
{
float min = 1.0;

for(int i = 0; i < 9; i++)
{
if(min > values[i])
min = values[i];
}

return min;
}


//CELL SHADING
void cellShading()
{

vec3 normal = texture(normalTex, passUV).xyz;
vec3 position = texture(positionTex, passUV).xyz;
vec3 color = texture(colorTex, passUV).xyz;


vec3 lightVec = pointLight - position;
vec3 lightDir = normalize(lightVec);

float fallOff = 1.0f - max(length(lightVec) / maxDist, 0.0);


float lightness = min(max(dot(normalize(normal), lightDir), 0.0) * texture(shadowTex, passUV).x, 1.0f);

float meh = 0.5f - shadow_border_threshold;
//lightness = clamp(meh + lightness * (1 - meh), 0, 1);

float steps = 3;
lightness = floor(lightness * steps + 1) / steps;


//lightness = texture(toonRamp, vec2(lightness, 0.5)).x;

//cellOutput = texture(normalTex, passUV);
cellOutput  = vec4(color * lightness, 1.0);
}

//Sobel edge detection on Depth-Buffer
vec4 detectEdgeDepth(){

mat4 invProj = inverse(projectionMatrix);

vec4 farVector = invProj * vec4(0,0,1,1);
vec4 nearVector = invProj * vec4(0,0,-1,1);

float f= -(farVector.z / farVector.w);          //needed for linearization
float n = -(nearVector.z / nearVector.w);

    float[9] values;
        values[0] = textureOffset(depthTex, passUV, ivec2(-1,  1)).x;
        values[1] = textureOffset(depthTex, passUV, ivec2(-1,  0)).x;
        values[2] = textureOffset(depthTex, passUV, ivec2(-1, -1)).x;

        values[3] = textureOffset(depthTex, passUV, ivec2( 0,  1)).x;
        values[4] = texture(depthTex, passUV).x;
        values[5] = textureOffset(depthTex, passUV, ivec2( 0, -1)).x;

        values[6] = textureOffset(depthTex, passUV, ivec2( 1,  1)).x;
        values[7] = textureOffset(depthTex, passUV, ivec2( 1,  0)).x;
        values[8] = textureOffset(depthTex, passUV, ivec2( 1, -1)).x;

        minDepth = (3 * n) / (f + n - getMinDepth(values) * (f-n));

    //initiate kernels in X- and Y-direction
    const float[9] kernelX = float[9](-1, -2, -1, 0, 0, 0, 1, 2, 1);
    const float[9] kernelY = float[9](-1, 0, 1, -2, 0, 2, -1, 0, 1);

    vec4 resultX = vec4(0,0,0,1), resultY = vec4(0,0,0,1);

    //apply the convolution in X- and Y-direction
    for(int i = 0; i < 9; i++){
    resultX += kernelX[i] * values[i];
    resultY += kernelY[i] * values[i];
    }

    float edge = vec4(sqrt(resultX * resultX + resultY * resultY)).x;

    float threshhold = threshholdDepth * (1 - minDepth);

    vec4 result = (edge < threshhold) ? vec4(1,1,1,1) : vec4(0,0,0,1);


    return result;
}

//Sobel edge detection on Diffuse Color
vec4 detectEdgeDiffuse(){

    vec4[9] values;
        values[0] = textureOffset(materialTex, passUV, ivec2(-1,  1));
        values[1] = textureOffset(materialTex, passUV, ivec2(-1,  0));
        values[2] = textureOffset(materialTex, passUV, ivec2(-1, -1));

        values[3] = textureOffset(materialTex, passUV, ivec2( 0,  1));
        values[4] = texture(materialTex, passUV);
        values[5] = textureOffset(materialTex, passUV, ivec2( 0, -1));

        values[6] = textureOffset(materialTex, passUV, ivec2( 1,  1));
        values[7] = textureOffset(materialTex, passUV, ivec2( 1,  0));
        values[8] = textureOffset(materialTex, passUV, ivec2( 1, -1));

         //initiate kernels in X- and Y-direction
         const float[9] kernelX = float[9](-1, -2, -1, 0, 0, 0, 1, 2, 1);
         const float[9] kernelY = float[9](-1, 0, 1, -2, 0, 2, -1, 0, 1);
         vec4 resultX = vec4(0,0,0,1), resultY = vec4(0,0,0,1);

         for(int i = 0; i < 9; i++){
         resultX += kernelX[i] * values[i].r;
         resultY += kernelY[i] * values[i].r;
         }

    float edge = vec4(sqrt(resultX * resultX + resultY * resultY)).x;
    vec4 result = (edge < threshholdDiffuse) ? vec4(1,1,1,1) : vec4(0,0,0,1);
    return result;
}

vec4 detectEdgeNormal(){
        vec3[9] values;
            values[0] = normalize(textureOffset(normalTex, passUV, ivec2(-1,  1)).xyz);
            values[1] = normalize(textureOffset(normalTex, passUV, ivec2(-1,  0)).xyz);
            values[2] = normalize(textureOffset(normalTex, passUV, ivec2(-1, -1)).xyz);

            values[3] = normalize(textureOffset(normalTex, passUV, ivec2( 0,  1)).xyz);
            values[4] = normalize(texture(normalTex, passUV).xyz);
            values[5] = normalize(textureOffset(normalTex, passUV, ivec2( 0, -1)).xyz);

            values[6] = normalize(textureOffset(normalTex, passUV, ivec2( 1,  1)).xyz);
            values[7] = normalize(textureOffset(normalTex, passUV, ivec2( 1,  0)).xyz);
            values[8] = normalize(textureOffset(normalTex, passUV, ivec2( 1, -1)).xyz);

         float resultDot = 0.0f;

        resultDot += dot(values[0], values[8]);
        resultDot += dot(values[1], values[7]);
        resultDot += dot(values[2], values[6]);
        resultDot += dot(values[3], values[5]);

        resultDot = resultDot / 4.0;

         vec4 result = vec4(1,1,1,1);

         if(resultDot < threshholdNormal)
            result = vec4(0,0,0,1);

         return result;
}

void main()
{
cellShading();

vec4 depthEdge = detectEdgeDepth();

sobelOutput = depthEdge * detectEdgeDiffuse() * detectEdgeNormal() + minDepth;
sobelOutput = min(sobelOutput, 1.0);
//sobelOutput = vec4(minDepth);
    //sobelOutput = detectEdgeNormal();
compositionOutput  = cellOutput * sobelOutput * texture(outLineTex, passUV);
}