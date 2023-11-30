#version 450

layout(location = 0) in vec2 inUV;

layout(location = 2) in flat int id;
layout(location = 0) out vec4 outColor;


void main() 
{
	float aId = id;
	outColor = vec4(0, 255, aId / 255, 1);
}