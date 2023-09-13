#version 450
layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const mat3 yuv2rgb = mat3(
    1.0, 1.0, 1.0,
    0.0, -0.343, 1.765,
    1.402, -0.711, 0.0
);

void main() {
    outColor = vec4(texture(texSampler, fragTexCoord).rgb, 1.0);
}