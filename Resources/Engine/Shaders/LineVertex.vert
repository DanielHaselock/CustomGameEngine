#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 color;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 vp;
} ubo;


void main() {
    gl_Position = ubo.vp * ubo.model * vec4(inPosition, 1.0);
    color = inColor;
}