#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in ivec4 inBoneIDs;
layout(location = 6) in vec4 inWeights;

layout(location = 0) out vec2 outUV;
layout(location = 2) out vec4 outColor;
layout(location = 6) out int OutHasAlbedo;
layout(location = 7) out int OutHasNormal;
layout(location = 8) out int OutHasMetallic;
layout(location = 9) out int OutHasRoughness;
layout(location = 10) out int OutHasAO;
layout(location = 11) out vec3 OutWorldPos;
layout(location = 14) out mat3 OutTangentBasis;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 viewProjection;
    vec4 color;
    int hasAlbedo;
    int hasNormal;
    int hasMetalic;
    int hasRoughness;
    int hasAO;
} ubo;

void main()
{
    gl_Position = ubo.viewProjection * ubo.model * vec4(inPosition, 1);

    outColor = ubo.color;
    OutHasAlbedo = ubo.hasAlbedo;
    OutHasNormal = ubo.hasMetalic;
    OutHasMetallic = ubo.hasMetalic;
    OutHasRoughness = ubo.hasRoughness;
    OutHasAO = ubo.hasAO;

    OutWorldPos = vec3(ubo.model * vec4(inPosition, 1));
    outUV = inUV;
    OutTangentBasis = mat3(ubo.model) * mat3(inTangent, inBitangent, inNormal);
}