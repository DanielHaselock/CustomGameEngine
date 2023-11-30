#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in ivec4 inBoneIDs;
layout(location = 6) in vec4 inWeights;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec4 outColor;
layout(location = 2) out int OutHasAlbedo;
layout(location = 3) out int OutHasNormal;
layout(location = 4) out int OutHasMetallic;
layout(location = 5) out int OutHasRoughness;
layout(location = 6) out int OutHasAO;
layout(location = 7) out vec3 OutWorldPos;
layout(location = 8) out mat3 OutTangentBasis;
layout(location = 11) out vec4 OutDir[2];
layout(location = 13) out vec4 OutPointDir[6];
layout(location = 19) out vec4 OutSpotDir[6];
layout(location = 27) out vec3 OutWorldPosSimple;



const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 viewProjection;
    mat4 finalBonesMatrices[MAX_BONES];
    vec4 color;
    int hasAnimation;
    int hasAlbedo;
    int hasNormal;
    int hasMetalic;
    int hasRoughness;
    int hasAO;
} ubo;

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

void computeDirLigthSpace(Light light, vec4 totalPosition, int idx)
{
    OutDir[idx] = light.vp * ubo.model * totalPosition;
}

void computePointLigthSpace(Light light, int idx)
{
    OutPointDir[idx] = vec4(light.pos, 1);
}

void computeSpotLigthSpace(Light light, vec4 totalPosition, int idx)
{
    OutSpotDir[idx] = light.vp * ubo.model * totalPosition;
}

void main()
{
    vec4 totalPosition = vec4(inPosition, 1);
    
    if (ubo.hasAnimation == 1)
    {
        totalPosition = vec4(0.0f);

        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(inBoneIDs[i] == -1) 
                continue;

            if(inBoneIDs[i] >= MAX_BONES) 
            {
                totalPosition = vec4(inPosition,1.0f);
                break;
            }

            vec4 localPosition = ubo.finalBonesMatrices[inBoneIDs[i]] * vec4(inPosition,1.0f);
            totalPosition += localPosition * inWeights[i];
        }
    }

    gl_Position = ubo.viewProjection * ubo.model * totalPosition;

    int dirIdx = 0;
    int pointIdx = 0;
    int spotIdx = 0;
    for (int i = 0; i < uboFrag.numLights; i++)
	{
        if (uboFrag.lights[i].lightType == 0)
        {
            computeDirLigthSpace(uboFrag.lights[i], totalPosition, dirIdx);
            dirIdx++;
        }
        else if (uboFrag.lights[i].lightType == 1)
        {
            computePointLigthSpace(uboFrag.lights[i], pointIdx);
            pointIdx++;
        }
		else if (uboFrag.lights[i].lightType == 2)
		{
            computeSpotLigthSpace(uboFrag.lights[i], totalPosition, spotIdx);
            spotIdx++;
        }
	}
    
    outColor = ubo.color;
    OutHasAlbedo = ubo.hasAlbedo;
    OutHasNormal = ubo.hasMetalic;
    OutHasMetallic = ubo.hasMetalic;
    OutHasRoughness = ubo.hasRoughness;
    OutHasAO = ubo.hasAO;
    
    OutWorldPos = vec3(ubo.model * totalPosition);
    outUV = inUV;
    OutTangentBasis = mat3(ubo.model) * mat3(inTangent, inBitangent, inNormal);

    OutWorldPosSimple = inPosition;
}