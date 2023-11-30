#version 450

layout(location = 0) in vec2 inUV;
layout(location = 2) in vec4 inColor;
layout(location = 4) in flat int inHasTexture;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main() 
{
	outColor = inColor;
	if (inHasTexture == 1)
		outColor = (texture(texSampler, inUV) * inColor);
}