#version 450

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

vec3 positions[6] = vec3[](
  vec3(0.5, 0.5, 0.0),
  vec3(-0.5, 0.5, 0.0),
  vec3(-0.5, -0.5, 0.0),
  vec3(-0.5, -0.5, 0.0),
  vec3(0.5, -0.5, 0.0),
  vec3(0.5, 0.5, 0.0)
);

vec3 colors[3] = vec3[](
  vec3(1.0, 0.0, 0.0),
  vec3(0.0, 1.0, 0.0),
  vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
    fragColor = colors[gl_VertexIndex % 3];
    fragTexCoord = vec2(positions[gl_VertexIndex].x + 0.5, positions[gl_VertexIndex].y + 0.5);
}