#version 450

layout(location = 0) in vec2 inUV;
layout(location = 2) in vec4 inColor;
layout(location = 6) in flat int inHasAlbedo;
layout(location = 7) in flat int inHasNormal;
layout(location = 8) in flat int inHasMetallic;
layout(location = 9) in flat int inHasRoughness;
layout(location = 10) in flat int inHasAO;
layout(location = 11) in vec3 inWorldPos;
layout(location = 14) in mat3 inTangentBasis;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texAlbedo;
layout(set = 2, binding = 0) uniform sampler2D texNormal;
layout(set = 3, binding = 0) uniform sampler2D texMetallic;
layout(set = 4, binding = 0) uniform sampler2D texRoughness;
layout(set = 5, binding = 0) uniform sampler2D texAO;

const float PI = 3.14159265359;
const float Epsilon = 0.00001;
const int NumLights = 1;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);

struct AnalyticalLight {
	vec3 direction;
	vec3 radiance;
};

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

void main() 
{
	AnalyticalLight lights[NumLights];
	lights[0].direction = vec3(0, 0, 1);
	lights[0].radiance = vec3(1, 1, 1);
	vec3 eyePosition = vec3(0, 0, -4);

	// Sample input textures to get shading model params.
	vec3 albedo = inColor.rgb;
	if (inHasAlbedo == 1)
		albedo = albedo + texture(texAlbedo, inUV).rgb;
	
	vec3 normalMap = vec3(0.5, 0.5, 1);
	if (inHasNormal == 1)
		normalMap = texture(texNormal, inUV).rgb;
	
	float metalness = 0;
	if (inHasMetallic == 1)
		metalness = texture(texMetallic, inUV).r;

	float roughness = 1;
	if (inHasRoughness == 1)
		roughness = texture(texRoughness, inUV).r;

	float ao = 1;
	if (inHasAO == 1)
		ao = texture(texAO, inUV).r;

	// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(eyePosition - inWorldPos);

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
	for(int i=0; i<NumLights; ++i)
	{
		vec3 Li = -lights[i].direction;
		vec3 Lradiance = lights[i].radiance;

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

	// Final fragment color.
	vec3 ambient = vec3(0.25) * albedo * ao;

	outColor = vec4(directLighting + ambient, inColor.a);
}