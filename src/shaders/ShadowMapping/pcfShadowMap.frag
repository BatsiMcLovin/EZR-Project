#version 330

uniform sampler2D shadowTex;
uniform sampler2D positionTex;
uniform sampler2D normalTex;

uniform mat4 shadowViewMatrix = mat4(1.0);
uniform mat4 shadowProjMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

uniform vec3 lightPosition;

uniform int pcfMaxRadius = 10;

in vec2 passUV;

out vec4 fragColor;

float depthBias(float depth)
{
return 0.00025;
}

float pcf(vec3 position, vec3 position_w, float normal_weight)
{

float visibility = 1.0;
ivec2 texSize = textureSize(shadowTex, 0);
vec2 resolution = 0.75 / texSize;

int hits = 0;

float bias = position.z + depthBias(position.z);

vec4 depthVec = viewMatrix * vec4(position_w,1.0);

float depth_weight = 1.0 - clamp(-depthVec.z * 0.2 + 0.1f, 0.0, 1.0);

int radius = int(pcfMaxRadius);

for(int i = -radius; i <= radius; i++)
{
    for(int j = -radius; j <= radius; j++)
    {
        vec2 tc = vec2(i, j) * resolution;

        float depth = texture( shadowTex, position.xy + tc).x;
        if (  depth >= bias)
        {
        hits++;
        }
    }
}

float width = (radius * 2.0) + 1.0;
float result = hits / (width * width);

return result * 0.5 + 0.5;
}

float normalOffset(vec3 normal, vec3 position)
{

vec3 lightDir = position - lightPosition;

float offset = 1.0 - abs((dot(normalize(lightDir), normalize(normal)))) ;

return offset;

}

float shadowMapping()
{
vec3 normal_w = texture(normalTex, passUV).xyz;
vec3 position_w = texture(positionTex, passUV).xyz;

float offset = normalOffset(normal_w, position_w);

vec4 position = shadowProjMatrix * shadowViewMatrix * vec4(position_w + normalize(normal_w) * (offset * 0.005), 1.0);

position = position/position.w;
position = position * 0.5 + 0.5;

if(position.x >= 1.0 || position.y >= 1.0 || position.x < 0.0 || position.y < 0.0 )
    return 1.0f;


float visibility = pcf(position.xyz, position_w, offset);

return visibility;

}

void main()
{
fragColor = vec4(shadowMapping());
}