#version 430


uniform samplerCube cubemapTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;

uniform mat4 viewMatrix = mat4(1.0f);

uniform bool useFresnel = true;

in vec2 passUV;
out vec4 fragcolor;

void main()
{
	vec4 n = transpose(inverse(viewMatrix)) * vec4(texture(normalTexture, passUV).xyz, 0);
	vec4 p = viewMatrix * texture(positionTexture, passUV);
	n.xyz = normalize(n.xyz);
	p.xyz = normalize(p.xyz);
	vec3 dir = (inverse(viewMatrix) * vec4(reflect(normalize(p.xyz), normalize(n.xyz)), 0)).xyz;
	dir = normalize(dir);
	float fresnel = 1.0;
        if(useFresnel)
        {
            float f0 = 0.1f;
            fresnel = f0 + (1.0f - f0) * pow(1.0f - dot(n.xyz, -p.xyz), 5);
        }
	fragcolor = texture(cubemapTexture, normalize(dir)) * fresnel;
}