#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in ivec4 inBoneIDs;
layout(location = 6) in vec4 inWeights;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 viewProjection;
} ubo;

void main()
{
    vec4 pos = vec4(inPosition.xyz, 1);
    gl_Position = ubo.viewProjection * ubo.model * pos;
}