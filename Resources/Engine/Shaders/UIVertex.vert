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
layout(location = 6) out int OutAsTexture;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 proj;
    mat4 model;
    vec4 color;
    int asTexture;
} ubo;

void main()
{
	gl_Position = ubo.proj * ubo.model * vec4(inPosition, 1);
    outUV = inUV;
    outColor = ubo.color;
    OutAsTexture = ubo.asTexture;
}