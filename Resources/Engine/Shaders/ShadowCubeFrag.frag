#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inLightPos;
layout (location = 2) in float outFarPlane;

layout (location = 0) out vec4 outFragColor;

void main() 
{
    vec3 lightVec = inPos - inLightPos;
    outFragColor = vec4(length(lightVec) / outFarPlane, 0, 0, 1);
}