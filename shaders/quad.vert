#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 5) in vec3 inUVW;

layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec3 fragUVW;

void main() {
    fragPos = vec4(inPosition.xy, 0.0f, 1.0f);
    fragPos.xyz = fragPos.xyz * 2.0f;
    gl_Position = fragPos;
    fragUVW = inUVW;
}