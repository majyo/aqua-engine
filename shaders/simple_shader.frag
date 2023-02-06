#version 450

layout (location = 0) in Varyings {
    vec3 positionWS;
    vec3 normalWS;
    vec3 color;
} vary;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec4 lightDirection;
    vec4 viewPosition;
} ubo;

layout (push_constant) uniform Push {
    mat4 modelMatrix;
    mat3 normalMatrix;
} push;

//const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, 1.0, 1.0));
const float DIFFUSE = 0.8;
const float SPECULAR = 0.65;
const float AMBIENT = 0.05;

void main() {
    vec3 viewPosWS = vec3(ubo.viewPosition);
    vec3 lightDirWS = vec3(ubo.lightDirection);
    vec3 normalWS = normalize(vary.normalWS);
    vec3 viewDirWS = normalize(viewPosWS - vary.positionWS);
    vec3 halfVectorWS = normalize((lightDirWS + viewDirWS) * 0.5);

    float diffuseComponent = max(dot(normalWS, lightDirWS), 0) * DIFFUSE;
    float specularComponent = pow(max(dot(normalWS, halfVectorWS), 0), 32) * SPECULAR;
    float lightIntensity = diffuseComponent + specularComponent + AMBIENT;
    vec3 color = lightIntensity * vary.color;
    outColor = vec4(color, 1.0);
//    outColor = vec4(dot(normalWS, lightDirWS) * 0.8, 0, 0, 1.0);
}
