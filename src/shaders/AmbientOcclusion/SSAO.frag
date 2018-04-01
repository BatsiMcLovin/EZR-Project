#version 330

uniform sampler2D normalTex;
uniform sampler2D positionTex;

uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

uniform int num_samples = 30;
uniform float border_threshold = 5.0f;
uniform float contrast = 1.0f;
uniform float radius = 0.05f;

in vec2 passUV;

vec3 hemisphere[100] = vec3[](
vec3(0.020499,0.106183,0.081038),
vec3(0.673588,-0.590180,0.444936),
vec3(0.168826,-0.194078,0.031502),
vec3(-0.186045,-0.005694,0.111274),
vec3(-0.039399,0.101038,0.048711),
vec3(-0.099399,-0.040381,0.015852),
vec3(-0.003251,0.007756,0.528837),
vec3(-0.098362,0.085261,0.089276),
vec3(0.066071,-0.147427,0.198585),
vec3(0.404789,0.520517,0.311717),
vec3(-0.180818,-0.205406,0.233137),
vec3(0.017640,-0.053872,0.082943),
vec3(-0.039146,-0.107872,0.059508),
vec3(-0.198491,0.033676,0.013508),
vec3(-0.024158,-0.004965,0.098721),
vec3(0.064933,0.424466,0.108318),
vec3(-0.541652,0.547354,0.048820),
vec3(-0.055484,-0.056650,0.117203),
vec3(0.044135,0.714601,0.698139),
vec3(-0.220557,0.079509,0.825481),
vec3(0.170089,0.710832,0.318568),
vec3(-0.080717,-0.083460,0.012628),
vec3(0.117045,0.025470,0.012089),
vec3(0.418567,-0.065588,0.165406),
vec3(0.020523,-0.116864,0.245026),
vec3(-0.020185,0.021709,0.126879),
vec3(0.914441,0.078282,0.397076),
vec3(-0.298020,0.025815,0.145983),
vec3(0.085922,0.211989,0.709558),
vec3(0.004883,-0.072124,0.599080),
vec3(0.027152,-0.017134,0.098608),
vec3(-0.175349,-0.254580,0.363224),
vec3(-0.096354,-0.212996,0.415611),
vec3(-0.373617,-0.505094,0.343368),
vec3(0.050420,0.123105,0.134159),
vec3(0.434058,0.522959,0.082301),
vec3(-0.005145,0.302326,0.123958),
vec3(-0.154381,-0.025523,0.066763),
vec3(0.029863,-0.009762,0.119716),
vec3(-0.008634,-0.083969,0.055992),
vec3(0.008404,0.082089,0.406655),
vec3(-0.874221,0.485016,0.022309),
vec3(0.065959,-0.076466,0.036016),
vec3(-0.196772,-0.040959,0.342171),
vec3(0.238631,-0.307841,0.384643),
vec3(0.186429,-0.350858,0.312486),
vec3(-0.147756,0.447262,0.522581),
vec3(0.002128,0.017778,0.533370),
vec3(0.013590,0.134598,0.010188),
vec3(-0.022393,-0.057020,0.121659),
vec3(-0.117934,-0.266940,0.532132),
vec3(-0.051459,0.054329,0.133646),
vec3(0.208980,-0.334836,0.206407),
vec3(-0.366749,0.242524,0.437374),
vec3(-0.061569,-0.141497,0.006230),
vec3(-0.069976,0.070111,0.017290),
vec3(0.025719,0.537599,0.125121),
vec3(-0.059674,-0.353959,0.769716),
vec3(-0.210007,0.153446,0.047386),
vec3(-0.323162,-0.470319,0.554058),
vec3(-0.131783,-0.218018,0.366448),
vec3(0.234055,-0.098919,0.397130),
vec3(-0.320193,0.061198,0.292528),
vec3(-0.042886,0.047652,0.976106),
vec3(0.332452,0.328965,0.297053),
vec3(0.416566,0.617750,0.371431),
vec3(-0.017112,-0.036110,0.958652),
vec3(0.159607,-0.030056,0.539807),
vec3(-0.208635,-0.325414,0.627069),
vec3(0.096703,0.042422,0.022062),
vec3(0.048974,0.024925,0.423189),
vec3(0.138671,0.038489,0.076325),
vec3(-0.176759,-0.306325,0.000124),
vec3(0.124450,-0.072101,0.555741),
vec3(-0.004404,-0.001921,0.416978),
vec3(-0.003809,-0.101483,0.013725),
vec3(-0.204130,-0.004684,0.930346),
vec3(0.094870,-0.073843,0.106621),
vec3(-0.075607,-0.074696,0.210135),
vec3(-0.556939,-0.546005,0.282001),
vec3(0.081335,-0.062242,0.057438),
vec3(-0.147126,0.051192,0.987792),
vec3(0.176501,-0.081346,0.050502),
vec3(0.004623,-0.062134,0.089182),
vec3(-0.346452,0.050473,0.830931),
vec3(-0.048940,-0.001471,0.447725),
vec3(0.088481,0.360016,0.568679),
vec3(-0.555554,0.037168,0.525010),
vec3(0.002880,0.000328,0.100318),
vec3(0.134441,-0.022721,0.020944),
vec3(0.000953,0.000556,0.110542),
vec3(0.029119,0.365014,0.041351),
vec3(0.322178,-0.471301,0.158082),
vec3(0.198887,0.471401,0.349628),
vec3(0.650491,0.190542,0.694333),
vec3(-0.147029,-0.214043,0.965696),
vec3(0.137884,0.022368,0.914491),
vec3(-0.114014,0.066983,0.451085),
vec3(-0.059762,-0.407392,0.462517),
vec3(-0.265095,0.845358,0.023849)


);


mat3 calcTangentSpace(vec3 tangent, vec3 bitangent, vec3 normal)
{
    mat3 tanSpace;

    tanSpace[0] = tangent;
    tanSpace[1] = bitangent;
    tanSpace[2] = normal;

    return tanSpace;
}
vec4 ssao()
{

ivec2 size = textureSize(positionTex, 0);
vec2 resolution = 1 / size;

//Tangent Space
vec3 normal = normalize((transpose(inverse(viewMatrix)) * texture(normalTex, passUV)).xyz);
vec3 bitangent = cross(normal, vec3(1.0, 0.0, 0.0));
vec3 tangent = cross(bitangent, normal);

mat3 tanSpace = calcTangentSpace(tangent, bitangent, normal);

vec3 pos = (viewMatrix * texture(positionTex, passUV)).xyz;

normal *= 0.01;

float occlusion = 0.0;

vec3 samp_c;
vec4 samp_i;

float samp_depth;

for(int i = 0; i < num_samples; i++)
{
    samp_c = pos + normal + (tanSpace * ( hemisphere[i] * radius));

    samp_i = projectionMatrix * vec4(samp_c, 1);
    samp_i /= samp_i.w;
    samp_i.xy = samp_i.xy * 0.5 + 0.5;

    samp_depth = (viewMatrix * texture(positionTex, samp_i.xy)).z;

    float diff = abs(samp_c.z - samp_depth);

    float dist_to_pos = abs(pos.z - samp_depth);
    float rangeCheck= 0.0;

    float weight = (1.0 - length(hemisphere[i]));


    float meh = dist_to_pos - (radius * border_threshold);
    if(dist_to_pos < radius * border_threshold)
        rangeCheck = clamp(meh, 1.0f, 0.0f);
    if( samp_c.z < samp_depth )
    {
        occlusion += rangeCheck ;
    }
}

occlusion = 1 - ((occlusion * contrast)/ num_samples);

return vec4(occlusion);
}

void main()
{

gl_FragColor = ssao();

}