#version 450

layout(location = 0) in vec2 inUV;
layout(location = 2) in vec4 inColor;
layout(location = 6) in flat int inAsTexture;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main() 
{
	outColor = inColor;
	if (inAsTexture == 1)
	{
		vec4 color = texture(texSampler, inUV);
		if (color.a != 0)
			color.a = outColor.a;

		outColor = vec4(color.rgb + outColor.rgb, color.a);
	}
}