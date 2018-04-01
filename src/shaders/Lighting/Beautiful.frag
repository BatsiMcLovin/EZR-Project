#version 330

uniform sampler2D normalTex;
uniform sampler2D positionTex;
uniform sampler2D colorTex;
uniform sampler2D specularTex;
uniform sampler2D ambientOcclusionTex;
uniform sampler2D shadowTex;
uniform sampler2D cubeMapReflectionsTex;

uniform mat4 viewMatrix = mat4(1.0);

uniform vec3 pointLight = vec3(10.0f); //TODO: dynamic

in vec2 passUV;

out vec4 fragColor;


float getAO()
{

float ao = 0.0f;

ao += textureOffset(ambientOcclusionTex, passUV, ivec2(-1,-1)).r;
ao += textureOffset(ambientOcclusionTex, passUV, ivec2(0,-1)).r;
ao += textureOffset(ambientOcclusionTex, passUV, ivec2(1,-1)).r;
ao += textureOffset(ambientOcclusionTex, passUV, ivec2(-1,0)).r;
ao += textureOffset(ambientOcclusionTex, passUV, ivec2(0,0)).r;
ao += textureOffset(ambientOcclusionTex, passUV, ivec2(1,0)).r;
ao += textureOffset(ambientOcclusionTex, passUV, ivec2(-1,1)).r;
ao += textureOffset(ambientOcclusionTex, passUV, ivec2(0,1)).r;
ao += textureOffset(ambientOcclusionTex, passUV, ivec2(1,1)).r;

return ao / 9.0;
}

void main()
{
vec4 normal = (texture(normalTex, passUV));
vec4 position = texture(positionTex, passUV);

vec3 view = normalize((viewMatrix * position).xyz);

vec4 color = texture(colorTex, passUV) * texture(shadowTex, passUV).x;
vec4 specular = texture(specularTex, passUV);

vec3 lightDir = normalize(pointLight - position.xyz);

vec3 reflection = reflect(lightDir, normal.xyz);

float diffuseTerm = max(dot(lightDir, normal.xyz), 0.0) * 0.5f;
float specularTerm = dot(normalize(reflection), view);

specularTerm = max(specularTerm, 0.0);

vec3 normal_cam = normalize((transpose(inverse(viewMatrix)) * normal).xyz);

float f0 = 0.1f;
float fresnel = f0 + (1.0f - f0) * pow(1.0f - dot(normal_cam, -view), 5);

float ambientOc = getAO();
vec4 cubeMapReflections = texture(cubeMapReflectionsTex, passUV);

fragColor = (color * (diffuseTerm + 0.5f)) + (specular * (vec4(pow(specularTerm, specular.a)) + cubeMapReflections)) * fresnel;

fragColor *= ambientOc;

}
