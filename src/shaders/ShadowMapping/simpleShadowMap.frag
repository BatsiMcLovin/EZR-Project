#version 330 core

uniform sampler2D shadowTex;
uniform sampler2D positionTex;
uniform sampler2D normalTex;

uniform mat4 shadowViewMatrix = mat4(1.0);
uniform mat4 shadowProjMatrix = mat4(1.0);
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 lightPosition;

in vec2 passUV;

float normalOffset(vec3 normal, vec3 position)
{

vec3 lightDir = position - lightPosition;

float offset = 1.0 - abs((dot(normalize(lightDir), normalize(normal)))) ;

return offset * 0.005;

}

float shadowMapping()
{

    vec3 normal_w = texture(normalTex, passUV).xyz;
    vec3 position_w = texture(positionTex, passUV).xyz;

    float offset = normalOffset(normal_w, position_w);

    vec4 position = shadowProjMatrix * shadowViewMatrix * vec4(position_w + normalize(normal_w) * offset, 1.0);

    position = position/position.w;
    position = position * 0.5 + 0.5;


    if(position.x >= 1.0 || position.y >= 1.0 || position.x < 0.0 || position.y < 0.0 )
        return 1.0f;

    float depthLookUp = texture(shadowTex, position.xy).x;

    if(depthLookUp < position.z + 0.00005)
        return 0.5;
    else
        return 1.0;

}
void main()
{
    gl_FragColor = vec4(shadowMapping());
}