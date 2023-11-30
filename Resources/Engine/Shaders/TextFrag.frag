#version 450

layout(location = 0) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main() 
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texSampler, inUV).r);
    outColor = inColor * sampled;
}