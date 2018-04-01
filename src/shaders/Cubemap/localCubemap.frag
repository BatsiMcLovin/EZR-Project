#version 430


uniform samplerCube cubemapTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;

uniform mat4 viewMatrix = mat4(1.0f);
uniform vec3 roomScale = vec3(5.0f);
uniform vec3 offset = vec3(0, 1.5, 0);

uniform bool useFresnel = true;

in vec2 passUV;
out vec4 fragcolor;



vec3 getIntersection(vec3 pos, vec3 dir)
{

	dir = normalize(dir);

	vec3 dirs[6] = {vec3(1,0,0),
					vec3(0,1,0),
					vec3(0,0,1),
					vec3(0,-1,0),
					vec3(-1,0,0),
					vec3(0,0,-1)};

    float lambdas[6];
    for(int i = 0; i < 6; i++)
    	{
    	    vec3 d = dirs[i];
    	    lambdas[i] = dot((roomScale * d) - pos, d) / dot(dir, d);
    	}

    	float biggest = 1000;
    	int wow = 0;
    for(int i = 0; i < 6; i++)
        {
            if(abs(lambdas[i]) < abs(biggest) && dot(dirs[i], dir) > 0)
            {
                biggest = lambdas[i];
                wow = i;
            }
        }

	return pos + dir * biggest;
}

void main()
{
    vec4 p_world = texture(positionTexture, passUV);
    vec4 n_world = texture(normalTexture, passUV);
	vec4 n = transpose(inverse(viewMatrix)) * vec4(n_world.xyz, 0);
	vec4 p = viewMatrix * p_world;
    n.xyz = normalize(n.xyz);
    vec3 p_norm = normalize(p.xyz);
	vec3 dir = normalize(reflect(p_norm, n.xyz));
	vec4 intersection = vec4(getIntersection(p_world.xyz - offset, (inverse(viewMatrix) * vec4(dir, 0)).xyz).xyz / roomScale, 0);
	vec4 newDir = intersection;
	fragcolor = intersection;
	//return;
	fragcolor = newDir;
	fragcolor = inverse(viewMatrix) * vec4(dir, 0);
    float fresnel = 1.0f;
    if(useFresnel)
    {
        float f0 = 0.1f;
        fresnel = f0 + (1.0f - f0) * pow(1.0f - dot(n.xyz, -p_norm), 5);
    }

	fragcolor = texture(cubemapTexture, normalize(newDir.xyz)) * fresnel;
}