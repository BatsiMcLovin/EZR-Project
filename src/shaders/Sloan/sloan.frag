#version 330

uniform sampler2D positionTex;
uniform sampler2D normalTex;
uniform sampler2D litSphereTex;

uniform mat4 viewMatrix;

in vec2 passUV;

out vec4 fragColor;

vec2 clampMagnitude(vec2 vec, float min, float max){

    float len = length(vec);
    return normalize(vec) * clamp(len, min, max);

}

void main() {
	vec4 pos_view = viewMatrix * texture(positionTex, passUV);
	vec4 norm_view = transpose(inverse(viewMatrix)) * texture(normalTex, passUV);

    float dotU = abs(dot(normalize(norm_view.xy), normalize(pos_view.xy)));
    float dotV = abs(dot(normalize(norm_view.yz), normalize(pos_view.yz)));

    fragColor = vec4(dotU, dotV, 0,0);
    vec2 uvs = clampMagnitude(norm_view.xy, 0.0f, 0.8f) * 0.5 + 0.5;

    fragColor = texture(litSphereTex, uvs);

}
