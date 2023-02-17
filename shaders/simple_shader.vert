#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 uv;

layout (location = 0) out Varyings {
    vec3 positionWS;
    vec3 normalWS;
    vec3 color;
} vary;

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec4 lightDirection;
    vec4 viewPosition;
} ubo;

layout (push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    vec4 positionWS = push.modelMatrix * vec4(position, 1.0);

    vary.positionWS = vec3(positionWS);
    vary.normalWS = mat3(push.normalMatrix) * normalize(normal);
    vary.color = color;

    gl_Position = ubo.projectionViewMatrix * positionWS;
}
