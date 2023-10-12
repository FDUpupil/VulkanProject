#version 450
layout(binding = 0) uniform sampler2D texSampler;
layout(binding = 1) uniform sampler2D texSamplerUV;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const mat3 yuv2rgb = mat3(
    1.0, 1.0, 1.0,
    0.0, -0.343, 1.765,
    1.402, -0.711, 0.0
);

void main() {
    // vec3 yuv = texture(texSampler, fragTexCoord).rgb;
    float y = texture(texSampler, fragTexCoord).r;
    float u = texture(texSamplerUV, fragTexCoord).r;
    float v = texture(texSamplerUV, fragTexCoord).g;

    float r = y + 1.403 * (v - 0.5);
    float g = y - 0.343 * (u - 0.5) - 0.714 * (v - 0.5);
    float b = y + 1.770 * (u - 0.5);
    
    outColor = vec4(r, g, b, 1.0);
}