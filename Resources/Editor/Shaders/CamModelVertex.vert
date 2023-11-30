#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in ivec4 inBoneIDs;
layout(location = 6) in vec4 inWeights;

layout(location = 0) out vec2 outUV;

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

    outUV = inUV;
}