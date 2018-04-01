#version 330 core

uniform sampler2D shadowTex;
uniform sampler2D positionTex;
uniform sampler2D normalTex;

uniform mat4 shadowViewMatrix = mat4(1.0);
uniform mat4 shadowProjMatrix = mat4(1.0);
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 lightPosition;

uniform int pcf_radius = 4;

uniform int numPoissonSamples = 30;

in vec2 passUV;

vec2 poissonDisk[100] = vec2[](

vec2(-0.243003,-0.061642),
vec2(0.146546,-0.052428),
vec2(-0.335198,-0.942098),
vec2(-0.387708,-0.059873),
vec2(-0.159352,0.106523),
vec2(-0.311090,-0.524465),
vec2(0.023492,-0.002173),
vec2(0.223055,-0.264583),
vec2(0.173495,0.141462),
vec2(0.712431,0.005865),
vec2(-0.204832,-0.190824),
vec2(0.049253,0.000142),
vec2(0.004353,0.108008),
vec2(-0.226563,-0.016688),
vec2(-0.214569,-0.551185),
vec2(0.153052,0.731380),
vec2(-0.769049,0.435204),
vec2(-0.231940,-0.290527),
vec2(-0.007064,0.016354),
vec2(0.117193,0.094267),
vec2(-0.172457,0.014137),
vec2(-0.050463,-0.735092),
vec2(0.033013,0.082515),
vec2(0.040343,0.178619),
vec2(0.050918,0.082367),
vec2(0.130425,-0.316163),
vec2(0.005416,0.014231),
vec2(0.052459,-0.002410),
vec2(-0.004582,-0.023677),
vec2(0.083078,-0.212278),
vec2(-0.360599,-0.208121),
vec2(0.150219,0.641736),
vec2(-0.242045,0.352110),
vec2(0.024063,0.097660),
vec2(0.068950,0.056784),
vec2(0.013170,0.011123),
vec2(-0.076814,0.109875),
vec2(0.025884,0.043772),
vec2(-0.404927,-0.061498),
vec2(-0.011405,0.058874),
vec2(-0.061863,-0.173489),
vec2(-0.072602,-0.016737),
vec2(-0.037197,-0.847728),
vec2(-0.002823,-0.414084),
vec2(-0.071023,0.039962),
vec2(-0.023440,-0.006276),
vec2(0.002942,-0.000674),
vec2(0.216919,-0.205133),
vec2(0.080180,-0.062834),
vec2(0.008638,-0.003431),
vec2(0.035950,0.161391),
vec2(-0.201823,-0.263135),
vec2(0.293676,0.588514),
vec2(0.473929,-0.184298),
vec2(0.034662,0.185348),
vec2(0.269411,0.145372),
vec2(0.017991,0.044188),
vec2(0.012762,-0.019701),
vec2(-0.060305,0.057570),
vec2(-0.198626,0.121425),
vec2(0.506924,-0.104865),
vec2(-0.058747,0.016510),
vec2(0.479015,0.455201),
vec2(0.044942,0.037175),
vec2(0.001331,0.045815),
vec2(0.055301,0.268591),
vec2(-0.068638,0.056901),
vec2(0.320998,0.031719),
vec2(-0.042100,0.084940),
vec2(-0.093046,-0.349333),
vec2(-0.008200,-0.379896),
vec2(0.132120,0.988368),
vec2(0.001716,0.000477),
vec2(0.001286,-0.070618),
vec2(-0.113530,-0.016697),
vec2(0.101438,0.312148),
vec2(0.025601,0.027373),
vec2(-0.108196,-0.034659),
vec2(-0.313938,-0.130153),
vec2(0.338279,0.334663),
vec2(-0.062293,-0.081190),
vec2(0.292337,0.389057),
vec2(0.044565,-0.000347),
vec2(-0.460945,0.126047),
vec2(0.019173,0.042656),
vec2(0.276108,0.047588),
vec2(0.106531,-0.090932),
vec2(0.131493,-0.179278),
vec2(-0.079168,0.112499),
vec2(0.279031,0.103912),
vec2(0.067116,-0.386808),
vec2(-0.341161,-0.577654),
vec2(0.166290,-0.836718),
vec2(-0.132582,0.077654),
vec2(0.034545,-0.023580),
vec2(-0.141116,0.029890),
vec2(0.157703,-0.006982),
vec2(0.049529,0.102698),
vec2(0.132534,-0.006972),
vec2(0.577526,0.012605)

);


float depthBias(float depth)
{
return 0.00025;
}

float normalOffset(vec3 normal, vec3 position)
{

vec3 lightDir = position - lightPosition;

float offset = 1.0 - abs((dot(normalize(lightDir), normalize(normal)))) ;

return offset;

}

float poisson(vec3 position, float normal_weight)
{
float visibility = 1.0;
ivec2 texSize = textureSize(shadowTex, 0);
vec2 resolution = 1.0 / texSize;

int num_samples = numPoissonSamples;
float subtractor = 1.0/num_samples;
float spreadage = 1500 - 1300.0 * normal_weight;

float bias = position.z + 0.00005;

 for (int i=0; i<num_samples; i++)
 {
    vec2 poisson_tc = position.xy + poissonDisk[i]/spreadage;
    float weighted_subtractor = (subtractor * (1.0 - sqrt(length(poissonDisk[i])) * 0.5));

   if ( texture( shadowTex, poisson_tc ).x  < bias)
   {
     visibility = visibility - weighted_subtractor;
   }

 }

return visibility;
}

float shadowMapping()
{

vec3 normal_w = texture(normalTex, passUV).xyz;
vec3 position_w = texture(positionTex, passUV).xyz;

float offset = normalOffset(normal_w, position_w);

vec4 position = shadowProjMatrix * shadowViewMatrix * vec4(position_w + normalize(normal_w) * offset * 0.005, 1.0);

position = position/position.w;
position = position * 0.5 + 0.5;

if(position.x >= 1.0 || position.y >= 1.0 || position.x < 0.0 || position.y < 0.0 )
return 1.0f;

float visibility = poisson(position.xyz, offset);

float low_value = 0.3f;
return low_value + (1.0f - low_value) * visibility;

}

void main()
{

gl_FragColor = vec4(shadowMapping());

}