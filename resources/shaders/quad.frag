#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec3 inUVW;

layout(push_constant) uniform constants {
    vec2 resolution;
} pc;

layout(location = 0) out vec4 outColor;

void main() {
//    uint xPos = uint((inPos.x + 1.0f) * 0.5f * pc.resolution.x);
//    uint yPos = uint((inPos.y + 1.0f) * 0.5f * pc.resolution.y);

    outColor = texture(texSampler, inUVW.xy);

//    float checker = float((xPos % 8 < 4) ^^ (yPos % 8 < 4));
//
//    outColor.xyz = outColor.xyz * checker;
}