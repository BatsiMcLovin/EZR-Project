#version 450
#define eps 0.00001f

in vec2 passUV;
out vec4 fragColor;

uniform sampler2D colorFrontFaces; 	// rauschtextur GL_TEXTURE0
uniform sampler2D colorBackFaces;  	// frontface culling GL_TEXTURE1
uniform sampler2D vectorData;      	// gradienten 2d projiziert auf view plane GL_TEXTURE2
uniform sampler2D vectorDataBack;	// GL_TEXTURE3
uniform sampler2D phongTexture;		// GL_TEXTURE4
uniform sampler2D contour;			// GL_TEXTURE5
uniform sampler2D depthContour;		// GL_TEXTURE6
uniform sampler2D depthTexture;		// GL_TEXTURE7
uniform sampler2D voronoiTexture;	// GL_TEXTURE8

uniform vec2 screenSize = vec2(1200, 800);
uniform mat4 projectionMatrix = mat4(1.0f);
uniform int LICIteration = 35;
uniform float viewSpaceMaxDistance = 1.0f;
uniform float stepFactor = 40;
uniform float lookUpDist = 8;
uniform float lineColor = 0.001f;
uniform float licContrast = 2.0f;
uniform float licAngle = 0.0f;
uniform float secondLicAngle = 0.0f;
uniform float input_dotRad = 1.0f;
uniform float smoothFactor = 1.0f;
uniform float wingRange = 1;

uniform bool viewSpaceDistance = true;
uniform bool stipplingLineConnection = false;

vec2 left = vec2(0,-1) * lookUpDist;
vec2 right = vec2(0,1) * lookUpDist;
vec2 up = vec2(1,0) * lookUpDist;
vec2 down = vec2(-1,0) * lookUpDist;

float cell_size = 1.0f;
vec4 viewSpacePos;

bool debugCondition = false;

// ------------------

vec4 getNextDir(vec2 coord, sampler2D image)
{
    vec4 result = vec4(0);
    result += texelFetch(image, ivec2(coord * screenSize + up ), 0);
    result += texelFetch(image, ivec2(coord * screenSize + down ), 0);
    result += texelFetch(image, ivec2(coord * screenSize + left ), 0);
    result += texelFetch(image, ivec2(coord * screenSize + right ), 0);
    result /= 4;
    return result;
}

vec4 getViewPos (vec3 fragCoord)
{
	fragCoord = fragCoord * 2.0 - 1.0;

	vec4 viewPos = inverse(projectionMatrix) * vec4(fragCoord, 1);
	viewPos.xyz = viewPos.xyz/ viewPos.w;
		
	return viewPos; 
}

vec4 getScreenPos (vec4 viewCoord)
{
	vec4 projectedPoint = projectionMatrix * viewCoord;
	projectedPoint.xyz = projectedPoint.xyz / projectedPoint.w;

	return projectedPoint;
}

vec2 rotate2D(vec2 vector, float angle)
{
	mat2 rotMatrix = mat2( 	cos(angle), -sin(angle), 
							sin(angle), cos(angle));
	
	return rotMatrix * vector;

}

float lineIntegration(vec2 stepSize, int lic_num, vec4 direction, float angle, bool invert)
{
    vec2 coord = passUV;
    vec2 centerCoord = vec2(0);
    float depthCenter = 0; 
    float weightSum = 0;
    float colorSum = 0;

	// ITERATION PARAMTERS
	
	float inverDirection = invert ? -1 : 1;
	stepSize *= invert ? -1 : 1;
	float flipDirection = 1.0f;

	// FOR PERPENDICULAR SMOOTHING

    float cell_size_half = cell_size * 0.5f;
    float maxWingRadius = clamp(wingRange, 0, 1) * cell_size_half * sqrt(2);
    vec4 projectedPoint = getScreenPos(vec4(maxWingRadius, 0, viewSpacePos.z, 1));	
	float projSpaceMaxDistance = min(projectedPoint.x, 1);

	// LIC ITERATIONS	
	
	for (int i = 0; i < lic_num; i++)
	{
		bool contour_hit = false;
				
		float depthFFstep = texelFetch(depthTexture, ivec2(coord * screenSize), 0).r - eps;
		
	    vec3 cellCenter = texture(voronoiTexture, passUV).xyz;
			

		if (depthFFstep < 1 && depthFFstep >= (texelFetch(depthContour, ivec2(coord * screenSize), 0)).r) // COMPARE DEPTHCONTOUR WITH DEPTH
		{
            contour_hit = (texelFetch(contour, ivec2(coord * screenSize), 0)).xyzw != vec4(1,1,1,1); // WAS CONTOUR HIT ?
        }

		if((coord.x < 0) || (coord.x >= 1) || (coord.y < 0) || (coord.y >= 1) || contour_hit)
		{
		   break;
        }
        
        
        vec4 colorOutput;
        
        colorOutput = texture(colorFrontFaces, coord);
        float colorValue = max(1.0f - colorOutput.g - 0.5f * colorOutput.r, 0);
        

		if(stipplingLineConnection) 	// PERPENDICULAR SMOOTHING
		{
			float factor = projSpaceMaxDistance * smoothFactor;
		
			vec2 perpDir = rotate2D(normalize(direction.xy) * factor, 1.57079632679) / screenSize;
			
			colorOutput = texture(colorFrontFaces, coord + perpDir);
			float weight = min(texture(voronoiTexture, coord + perpDir).w / maxWingRadius, 1) * colorOutput.g;
        	colorValue += max(1.0f - colorOutput.g - 0.5f * colorOutput.r, 0) * pow(1 - weight, 10);
			
			colorOutput = texture(colorFrontFaces, coord - perpDir);
			weight = min(texture(voronoiTexture, coord - perpDir).w / maxWingRadius, 1) * colorOutput.g;
        	colorValue += max(1.0f - colorOutput.g - 0.5f * colorOutput.r, 0) * sqrt(1 - weight);
        
        weightSum += 2 * (1 - (i / lic_num));
        		
		}       
        
		colorSum += colorValue;
		weightSum += 1 ;
	
		
    	coord += vec2(stepSize.x * direction.x, stepSize.y * direction.y);


		// FLIP FACE DIRECTION IF CONTOUR IS REACHED //TODO: do it right
		if((texelFetch(depthTexture, ivec2(coord * screenSize), 0)).r <= 0) // OUTSIDE MESH SURFACE? CONTINUE FROM BACKFACE //TODO: == 0 ???
		{
		
        //    coord -= vec2(stepSize.x * direction.x, stepSize.y * direction.y);
            flipDirection *= -1;
        }
        
        	direction = flipDirection * getNextDir(coord, vectorData);
			direction.xy = rotate2D(direction.xy, licAngle + angle);
  
					
	}
	
	return colorSum / weightSum;
}

void main()
{

	// CONTOUR (return, once you know it's a contour pixel)
	float depthFF   = texelFetch(depthTexture, ivec2(gl_FragCoord.xy), 0).r;
    float depth_contour   = texelFetch(depthContour, ivec2(gl_FragCoord), 0).r;
    
    if(depthFF > depth_contour)	// CONTOUR PIXEL ?
    {
        fragColor = vec4(lineColor); 
    	return;
    }
    if(abs(depthFF) >= 1.0f)	// BACKGROUND PIXEL ?
    {
        fragColor = vec4(1, 1, 1, 0); 
    	return;
    }
    vec4 gradientDir = texelFetch(vectorData, ivec2(gl_FragCoord), 0);
    gradientDir.xy = rotate2D(gradientDir.xy, licAngle);
    
    vec4 viewSpacePos = getViewPos(vec3(passUV, depthFF));
    cell_size = 4 * input_dotRad;
    vec4 cellCenter = texture(voronoiTexture, passUV);
	
	
  
	// NEITHER CONTOUR NOR CONTOUR PIXEL (go on with Line Integration)
    vec2 stepSize;
    int lic_num;


	//EXPERIMANTAL SHIT
	
	vec3 forwardCellScreenPos = getScreenPos(viewSpacePos + vec4(normalize(gradientDir.xyz), 0) * cell_size * 0.5).xyz;
	vec3 backwardCellScreenPos = getScreenPos(viewSpacePos - vec4(normalize(gradientDir.xyz), 0) * cell_size * 0.5).xyz;

	
	float wow = 0;
	vec2 dick = vec2(1,1);
	float datAngle = 0;

	if(viewSpaceDistance)
	{
		vec4 projectedPoint = getScreenPos(vec4(viewSpaceMaxDistance, 0, viewSpacePos.z, 1));
		
		float projSpaceMaxDistance = min(projectedPoint.x, 1);
		float screenSpaceMaxDistance = projSpaceMaxDistance * min(screenSize.x, screenSize.y);  
		
		stepSize = vec2(1) / screenSize;
		lic_num = int(screenSpaceMaxDistance);
		
	}
	else
	{
		stepSize = (stepFactor) / screenSize;
		lic_num = LICIteration;
	}


	// LINE INTEGRATION
	
	float colorResult = lineIntegration(stepSize, lic_num, gradientDir, 0.0 , false);
	colorResult += lineIntegration(stepSize, lic_num, gradientDir, 0.0f, true);
	
	float divisor = 2.0f;
	
	if(secondLicAngle > 0.0f)
	{
	colorResult += lineIntegration(stepSize, lic_num, gradientDir, secondLicAngle, false);
	colorResult += lineIntegration(stepSize, lic_num, gradientDir, secondLicAngle, true);
	divisor = 4.0f;
	}
		
	colorResult /= divisor;
		
		
	// LIC DONE, CALCULATE COLOR VALUE
    if(lic_num > 0)
	{
	        fragColor = vec4(1.0f - (colorResult)); 
    }
    else // IN CASE THE FOR-LOOP WILL NOT BE EXECUTED
    {
    	vec4 frontFaceColor = texture(colorFrontFaces, passUV, depthFF);
        float frontFaceColor_value = (frontFaceColor.g + frontFaceColor.r) * 0.5f;
        fragColor = vec4(frontFaceColor_value);
    }


	// ENHANCEMENT
    const float contrastEnhance = 0.8;
    vec4 fragColor_temp = (fragColor + contrastEnhance);
    fragColor = ((fragColor_temp * fragColor_temp) - (contrastEnhance * contrastEnhance)) / (2 * contrastEnhance + 1);


	// PHONG MIXER
	vec4 phongValues = texelFetch(phongTexture, ivec2(gl_FragCoord), 0);
    float phongTransition = phongValues.r;
    float fallOff = phongValues.g;  // specular falloff is stored in depth alpha
    float diffuseColor = phongValues.b; // diffuse lighting
    
    vec4 phongColor = mix(fragColor, vec4(1), diffuseColor); // MIX DIFFUSE
    phongColor = mix(vec4(1), phongColor, fallOff); // MIX FALLOFF
    
    vec4 colorA = mix(fragColor, vec4(diffuseColor), round(phongTransition * 0.5f)); //MAYBE REMOVE LATER...
    
    vec4 mixColor = mix(colorA, phongColor, (1.0f - abs(phongTransition - 1.0f )));  
	
	fragColor = phongColor;
   // even more black-white contrast
   fragColor = pow(fragColor, vec4(licContrast * (1 + viewSpaceMaxDistance)));
    
   fragColor.w = colorResult > 0.000001f ? wow : 0; // Debugging stuff
  
   }
