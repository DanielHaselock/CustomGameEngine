#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in ivec4 inBoneIDs;
layout(location = 6) in vec4 inWeights;

layout (location = 0) out vec3 outPos;
layout (location = 1) out vec3 outLightPos;
layout (location = 2) out float outFarPlane;

layout(set = 0, binding = 0) uniform UniformBufferObject2 
{
    vec3 ligthPos;
    float farPlane;
} uboLight;

layout( push_constant ) uniform UniformBufferObject 
{
    mat4 vp;
    mat4 model;
} ubo;

void main() 
{
    gl_Position = ubo.vp * ubo.model * vec4(inPosition, 1);
    outPos = vec3(ubo.model * vec4(inPosition, 1));
    outLightPos = uboLight.ligthPos;
    outFarPlane = uboLight.farPlane;
}