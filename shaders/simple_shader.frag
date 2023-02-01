#version 450

layout (location = 0) in Varying {
    vec3 normalWS;
    vec3 color;
} vary;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform Push {
    mat4 transform;
    mat3 normalMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, 1.0, 1.0));
const float AMBIENT = 0.02;

void main() {
    vec3 normalWS = normalize(vary.normalWS);
    float lightIntensity = max(dot(normalWS, DIRECTION_TO_LIGHT), 0) + AMBIENT;
    vec3 color = lightIntensity * vary.color;
    outColor = vec4(color, 1.0);
}
