#version 450

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;
layout(location = 2) in flat int inHasAlbedo;
layout(location = 3) in flat int inHasNormal;
layout(location = 4) in flat int inHasMetallic;
layout(location = 5) in flat int inHasRoughness;
layout(location = 6) in flat int inHasAO;
layout(location = 7) in vec3 inWorldPos;
layout(location = 8) in mat3 inTangentBasis;
layout(location = 11) in vec4 inDir[2];
layout(location = 13) in vec4 inPointDir[6];
layout(location = 19) in vec4 inSpotDir[6];
layout(location = 27) in vec3 inWorldPosSimple;

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform sampler2D pbrTexture[5];
layout(set = 3, binding = 0) uniform sampler2D shadowTexture;
layout(set = 4, binding = 0) uniform samplerCube shadowCubeTexture;

struct Light
{
	mat4 vp;
	vec3 radiance;
	vec3 direction;
	vec3 pos;
	int lightType;
	float attenuation;
	float radius;
	float brightness;
	float cutOff;
	float outerCutOff;
	int castShadow;
	int id;
};

const int MAX_LIGHT = 30;
layout(set = 1, binding = 0) uniform UniformBufferObject2 
{
	Light lights[MAX_LIGHT];
    vec3 eyePosition;
	vec4 shadowSize;
	vec4 shadowCubeSize;
	int shadowSplit;
	int shadowCubeSplit;
	int numLights;
} uboFrag;

const float PI = 3.14159265359;
const float Epsilon = 0.00001;

//Directionnal Light
//------------------
//------------------
//------------------
//------------------
// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);

float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}
// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}
// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}
// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 calculateDirectionnalLight(Light light, vec3 albedo, vec3 normalMap, float metalness, float roughness)
{
// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(uboFrag.eyePosition - inWorldPos);

	// Get current fragment's normal and transform to world space.
	vec3 N = normalize(2.0 * normalMap - 1.0);
		N = normalize(inTangentBasis * N);

	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));
		
	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(Fdielectric, albedo, metalness);

	// Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);

	{
		vec3 Li = -light.direction;
		vec3 Lradiance = light.radiance * light.brightness;;

		// Half-vector between Li and Lo.
		vec3 Lh = normalize(Li + Lo);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));

		// Calculate Fresnel term for direct lighting. 
		vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		// Calculate normal distribution for specular BRDF.
		float D = ndfGGX(cosLh, roughness);
		// Calculate geometric attenuation for specular BRDF.
		float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
		vec3 diffuseBRDF = kd * albedo;

		// Cook-Torrance specular microfacet BRDF.
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// Total contribution for this light.
		directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	}

	return directLighting;
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//Point Light
//------------------
//------------------
//------------------
//------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickPoint(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float sqr(float x)
{
	return x * x;
}


float attenuate_cusp(float distance, float radius, float max_intensity, float falloff)
{
	float s = distance / radius;

	if (s >= 1.0)
		return 0.0;

	float s2 = sqr(s);

	return max_intensity * sqr(1 - s2) / (1 + falloff * s);
}

vec3 calculatePointLight(Light light, vec3 albedo, vec3 normalMap, float metalness, float roughness)
{
	vec3 N = normalize(2.0 * normalMap - 1.0);
		N = normalize(inTangentBasis * N);

    vec3 V = normalize(uboFrag.eyePosition - inWorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metalness);

    // reflectance equation
    vec3 Lo = vec3(0.0);
	{
		// calculate per-light radiance
        vec3 L = normalize(light.pos - inWorldPos);
        vec3 H = normalize(V + L);
        float distance = length(light.pos - inWorldPos);
        vec3 radiance = light.radiance * attenuate_cusp(distance, light.radius, light.brightness, 4);

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlickPoint(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metalness;	  
        
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	return Lo;
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//Spot Light
//------------------
//------------------
//------------------
//------------------
// ----------------------------------------------------------------------------
vec3 fresnelSchlickSpot(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 calculateSpotLight(Light light, vec3 albedo, vec3 normalMap, float metalness, float roughness)
{
	vec3 N = normalize(2.0 * normalMap - 1.0);
		N = normalize(inTangentBasis * N);

    vec3 V = normalize(uboFrag.eyePosition - inWorldPos);

	vec3 L = normalize(light.pos - inWorldPos);
    vec3 H = normalize(V + L);

	float theta = dot(L, -light.direction);
    float attenuation = smoothstep(light.outerCutOff, light.cutOff, theta);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metalness);

    // reflectance equation
	vec3 Lo = vec3(0);
	{
		float NDF = DistributionGGX(N, H, roughness);
		float G   = GeometrySmith(N, V, L, roughness);
		vec3 F    = fresnelSchlickSpot(clamp(dot(H, V), 0.0, 1.0), F0);

		vec3 kD = (vec3(1.0, 1.0, 1.0) - F) * (1.0 - metalness);

		float NdotL = max(dot(N, L), 0.0);

		vec3 numerator = NDF * G * F;
		float denominator = max(4.0 * max(dot(N, V), 0.0) * NdotL, 0.001);
		vec3 specular = numerator / denominator;
	
		// add to outgoing radiance Lo
		vec3 radiance = light.radiance * attenuation * light.brightness;

		Lo = (kD * albedo / vec3(PI, PI, PI) + specular) * radiance * NdotL;
	}

	return Lo;
}
//------------------
//------------------
//------------------
//------------------
//------------------
float computeShadowFactor(vec4 lightSpacePos, int idx, int castShadow)
{
	vec3 lightSpaceNdc = lightSpacePos.xyz /= lightSpacePos.w;
 
	if ((abs(lightSpaceNdc.x) > 1.0 ||
		abs(lightSpaceNdc.y) > 1.0 ||
		abs(lightSpaceNdc.z) > 1.0))
		return 1;


	vec2 shadowMapCoord = lightSpaceNdc.xy * 0.5 + 0.5;

	vec2 uv = (shadowMapCoord.xy * uboFrag.shadowSize.xy) / uboFrag.shadowSize.zw;
	uv.x += ((idx % uboFrag.shadowSplit) * uboFrag.shadowSize.x) / uboFrag.shadowSize.z;
	uv.y += (int(idx / uboFrag.shadowSplit) * uboFrag.shadowSize.y) / uboFrag.shadowSize.w;

	float x = texture(shadowTexture, uv).x;
	if (lightSpaceNdc.z > x && castShadow == 1)
		return 0.0;
	return 1.0;


//    float shadow = 0.0;
//    vec2 texelSize = 1.0 / textureSize(shadowTexture, 0);
//    for(int x = -1; x <= 1; ++x)
//    {
//        for(int y = -1; y <= 1; ++y)
//        {
//            float pcfDepth = texture(shadowTexture, shadowMapCoord.xy + vec2(x, y) * texelSize).x; 
//            shadow += lightSpaceNdc.z > pcfDepth ? 0.0 : 1.0;        
//        }    
//    }
//    return shadow /= 9.0;
}  

float computeShadowFactorPoint(vec4 lightSpacePos, float farPLane, int castShadow)
{
	if (castShadow == 0)
		return 1;

	vec3 worldPos = vec3(-inWorldPos.x,inWorldPos.y, inWorldPos.z);
    vec3 lightVec = (worldPos - lightSpacePos.xyz);
    float sampledDist = texture(shadowCubeTexture, lightVec).r;
	sampledDist *= farPLane;

    float dist = length(lightVec);
	// Check if fragment is in shadow

	float bias = 0.05;
	float shadow = dist -  bias > sampledDist ? 0 : 1;

    return shadow;
}
//------------------
//------------------
//------------------
//------------------
void main() 
{
	// Sample input textures to get shading model params.
	vec3 albedo = inColor.rgb;
	if (inHasAlbedo == 1)
		albedo = albedo + texture(pbrTexture[0], inUV).rgb;
	
	vec3 normalMap = vec3(0.5, 0.5, 1);
	if (inHasNormal == 1)
		normalMap = texture(pbrTexture[1], inUV).rgb;
	
	float metalness = 0;
	if (inHasMetallic == 1)
		metalness = texture(pbrTexture[2], inUV).r;

	float roughness = 1;
	if (inHasRoughness == 1)
		roughness = texture(pbrTexture[3], inUV).r;

	float ao = 1;
	if (inHasAO == 1)
		ao = texture(pbrTexture[4], inUV).r;

	
	float attenuation = 0;
	vec3 directLighting = vec3(0);
	vec3 pointLighting = vec3(0);
	vec3 spotLighting = vec3(0);
	float shadowFactor = 0;
	int dirIdx = 0;
    int pointIdx = 0;
    int spotIdx = 0;
	bool shadow = false;
	for (int i = 0; i < uboFrag.numLights; i++)
	{
		if (uboFrag.lights[i].lightType == 0)
		{
			float res = computeShadowFactor(inDir[dirIdx], uboFrag.lights[i].id, uboFrag.lights[i].castShadow);
			directLighting += calculateDirectionnalLight(uboFrag.lights[i], albedo, normalMap, metalness, roughness);
			dirIdx++;

			if (res == 0)
				shadow = true;
		}
		else if (uboFrag.lights[i].lightType == 1)
		{
			float res = computeShadowFactorPoint(inPointDir[pointIdx], uboFrag.lights[i].radius, uboFrag.lights[i].castShadow);
			pointLighting += calculatePointLight(uboFrag.lights[i], albedo, normalMap, metalness, roughness);
			pointIdx++;

			if (res == 0)
				shadow = true;
		}
		else if (uboFrag.lights[i].lightType == 2)
		{
			float res = computeShadowFactor(inSpotDir[spotIdx], uboFrag.lights[i].id, uboFrag.lights[i].castShadow);
			spotLighting += calculateSpotLight(uboFrag.lights[i], albedo, normalMap, metalness, roughness);
			spotIdx++;

			if (res == 0)
				shadow = true;
		}

		//Attenuation
		if (attenuation < uboFrag.lights[i].attenuation)
			attenuation = uboFrag.lights[i].attenuation;
	}
	
	//Final shadow
	shadowFactor = shadow ? 0 : 1;
	
	// Final ambient color
	vec3 ambient = vec3(attenuation) * albedo * ao;

	outColor = vec4(((directLighting + pointLighting + spotLighting) * shadowFactor) + ambient, inColor.a);
}