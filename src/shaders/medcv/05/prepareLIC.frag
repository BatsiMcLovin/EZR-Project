#version 450
#define PI 3.14159265359f
#define PI_half 1.57079632679f

in vec4 passCurv_c_geom;
in vec4 passWorldVector_geom;
in vec4 passPosition_geom;
in vec4 passNormal_geom;
in vec4 passPosition_w_geom;
in float passDeriv_geom;
in vec4 passNormal_w_geom;
in vec3 passWorldSurfaceNormal_geom;

// coloring convention:
// .r = stroke color
// .g = dampening value
out layout(location = 0) vec4 color;
out layout(location = 1) vec4 viewProjectedVector;
out layout(location = 2) vec4 phongOutput;
out layout(location = 3) vec4 voronoiOutput;

uniform mat4 projectionMatrix = mat4(1.0f);
uniform mat4 viewMatrix = mat4(1.0f);
uniform float radial_distance_thresh = 0.1f;
uniform float radialDistanceReverseTransition = 0.1f;
uniform float outerStrokeColor = 1.0f;
uniform float featureSize = 1.0f;

uniform float input_dotRad = 2.0f;
uniform float coreSize = 1.0f;


uniform float wingRange = 3;
uniform float permeabilityFact = 4;
uniform float blendCurvatureFact = 1;
uniform float specPow = 2;


uniform int snapGradientAmount = 0;

// PHONG PARAMETERS

uniform float phongTransition = 0;
uniform vec3 lightDirection = vec3(1.0f);


vec3 viewDirection = vec3(0, 0, -1);

// ------------------

void main()
{
    viewDirection = (viewMatrix * vec4(0, 0, 1, 0)).xyz;
    vec3 position_w = passPosition_w_geom.xyz;

    vec2 projVec = normalize(passCurv_c_geom.xy);
    if(snapGradientAmount > 0)
	    projVec = ((round(projVec * snapGradientAmount) / float(snapGradientAmount)) * 0.5f + projVec * 0.5f) ;
    projVec = normalize(projVec);
    viewProjectedVector = vec4(projVec, 0, 0);

    vec4 clip_space_pos = projectionMatrix * passPosition_geom;
    float ndc_depth = clip_space_pos.z / clip_space_pos.w;
    float depthVal = 0.5 * ndc_depth + 0.5;
    phongOutput = vec4(0);

    float f = 3 - clamp(abs(dot(viewDirection, passNormal_w_geom.xyz)) * 2, 0, 2);
    float dotRad = input_dotRad;
    float cell_size = dotRad * 4;
    float cell_size_half = dotRad * 2;
    dotRad *= f;

    // find out in which cube cell this fragment is located
    int cell_x = int(position_w.x / cell_size);
    int cell_y = int(position_w.y / cell_size);
    int cell_z = int(position_w.z / cell_size);
    ivec3 cellIndex = ivec3(cell_x, cell_y, cell_z);

    if ( position_w.x < 0)
        cellIndex.x -= 1;
    if ( position_w.y < 0)
        cellIndex.y -= 1;
    if ( position_w.z < 0)
        cellIndex.z -= 1;

    vec3 cell_center = ivec3(cell_x, cell_y, cell_z) * cell_size + vec3(cell_size_half);
    
    // check which plane this fragment will be projected on
    // -> max(fragNormal DOT planeNormal)

    // 6 possible plane normals and cell face centers
    vec3 planeNormals[6] = {vec3(1,0,0), vec3(-1,0,0), vec3(0,1,0), vec3(0,-1,0), vec3(0,0,1), vec3(0,0,-1)};
    vec3 faceCenters[6] = {cell_center + vec3(cell_size_half, 0, 0), cell_center - vec3(cell_size_half, 0, 0),
                           cell_center + vec3(0, cell_size_half, 0), cell_center - vec3(0, cell_size_half, 0),
                           cell_center + vec3(0, 0, cell_size_half), cell_center - vec3(0, 0, cell_size_half)};

    int faceID = 0;
    float maxDot = 0;
    vec3 fragNormal = normalize(passNormal_w_geom.xyz);
    for(int i = 0; i < 6; i++)
    {
        float dotVal = (dot(fragNormal, planeNormals[i]));
        if (dotVal >= maxDot)
        {
            maxDot = dotVal;
            faceID = i;
        }
    }

    vec3 planeNormal = planeNormals[faceID];
    vec3 faceCenter = faceCenters[faceID];   
 
    // project the fragment on the found plane, using the direction of the fragment's normal
    float offset_distance = dot(-planeNormal, faceCenter - position_w) / dot(-planeNormal, fragNormal);
    vec3 projPoint = position_w + offset_distance * fragNormal;


    // if the projected point is outside the cell map the point to the same relative position in the current cell
    vec3 distToCell = projPoint - faceCenter;

    if (distToCell.x > cell_size_half)
        projPoint.x -= cell_size;
    if (distToCell.x < -cell_size_half)
        projPoint.x += cell_size;
    if (distToCell.y > cell_size_half)
        projPoint.y -= cell_size;
    if (distToCell.y < -cell_size_half)
        projPoint.y += cell_size;
    if (distToCell.z > cell_size_half)
        projPoint.z -= cell_size;
    if (distToCell.z < -cell_size_half)
        projPoint.z += cell_size;
      
    float dist = length(faceCenter - projPoint.xyz);
    
    //PORJECT THE CENTER ONTO THE PIXELS PLANE
    voronoiOutput = viewMatrix * vec4(position_w + (faceCenter - projPoint.xyz), 1);
    voronoiOutput.w = dist;
    	
    // the gradient has to be projected into the cell plane aswell
    vec3 projGradient = passWorldVector_geom.xyz * (vec3(1) - abs(planeNormal));
    projGradient = normalize(projGradient);

    vec3 dotEdge1 = faceCenter.xyz + normalize(projGradient.xyz) * dotRad / 4;
    vec3 dotEdge2 = faceCenter.xyz - normalize(projGradient.xyz) * dotRad / 4;

    // the fragment is part of the wing if (fragPos -> dotEdge1/2) DOT gradientDir yield different signs
    vec3 fragTo1 = normalize(dotEdge1 - projPoint.xyz);
    vec3 fragTo2 = normalize(dotEdge2 - projPoint.xyz);

    bool isValid_wing = dot(fragTo1, projGradient.xyz) * dot(fragTo2, projGradient.xyz) <= 0; // SEPERATED BY PLANE OF PROJGRADIENT ?

    // the fragment is part of the wing if (fragPos -> dotEdge1/2) DOT gradientDir yield different signs
    bool isValid_halo = isValid_wing; // FOR SOME REASON THIS WAS DECLARED AS SOMETHING ELSE (eventhough it's the same thing)

	float maxWingRadius = clamp(wingRange, 0, 1) * cell_size_half * sqrt(2);
	
    if(dist < maxWingRadius)
    {
        if (isValid_halo)
        {
        	float dotRadhalf = (dotRad * coreSize)/2;
        
        
        	float gradient = clamp(outerStrokeColor * ((dist - dotRadhalf) / dotRadhalf), 0, 1);
            color = vec4(max(gradient, 0) ,0 ,0 ,0);
        }
        else
            color = vec4(1,1,0,0);
    }
    else 
        color = vec4(1,1,0,0);

    // dot product with the unnormalized k_1 -> curvature dependent result
    // ->> sign is crucial for blending
    float vectorDotLight = (dot(passWorldVector_geom.xyz, viewDirection));
    float angle1 = acos( dot(passNormal_w_geom.xyz, normalize(-viewDirection)));

    float radial_distance_thresh_ = mix(radial_distance_thresh, PI_half, radialDistanceReverseTransition);

    float angle_weight1 =  clamp((angle1 - radial_distance_thresh_) / ( PI_half - radial_distance_thresh_), 0, 1);
    phongOutput.r = phongTransition;
    phongOutput.g = max(angle_weight1 * vectorDotLight * featureSize, blendCurvatureFact);

    phongOutput.b = 1;

    // ONLY IF PHONG SHADING IS ON
    if(phongTransition >= 0.0001f)
    {
	    vec3 normal = normalize(passNormal_geom.xyz);
	    vec3 lightDirection = normalize(vec3(lightDirection));
	    float diffuseColor = max(dot(normal, lightDirection), 0.0f);

		float phongTransition = clamp(phongTransition, 0.0f, 1.0f);
	    float radialPhongDistanceTransition = (1 - phongTransition) * PI_half;
	    float angle_weight2 =  clamp((radialPhongDistanceTransition - angle1) / ( PI_half - radialPhongDistanceTransition), -1.0f, 0.0f);
	    float derivativeWeight = mix(vectorDotLight * featureSize, -1.0f, phongTransition);
	    diffuseColor = mix(1, diffuseColor, clamp(angle_weight2 * derivativeWeight, 0, 1));
	
	    phongOutput.g = max(phongOutput.g, 0) * (1.0f - radialDistanceReverseTransition);

	    // store specular falloff in phongOutput green channel
	    float fSpecularFactor = max(dot(normalize(position_w), normalize(reflect(-lightDirection, normal))),0.0f);

	    fSpecularFactor = pow(fSpecularFactor,4 );
	    diffuseColor += fSpecularFactor * 0.05f + specPow /30;

	    diffuseColor = clamp(diffuseColor, 0.0f, 1.0f);
	    phongOutput.b = diffuseColor;
    }

}
