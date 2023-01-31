#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 uv;

layout (location = 0) out Varying {
    vec3 normalWS;
    vec3 color;
} vary;

layout (push_constant) uniform Push {
    mat4 transform;
    mat4 model;
} push;

void main() {
    vary.normalWS = normalize(mat3(push.model) * normal);
    vary.color = color;

    gl_Position = push.transform * vec4(position, 1.0);
}
