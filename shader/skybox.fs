#version 330 core
out vec4 FragColor;

in vec3 vWorldPos;

uniform samplerCube uEnvironmentMap;

void main() {
    vec3 color = textureLod(uEnvironmentMap, vWorldPos, 0.0).rgb;

    // HDR tonemap and gamma correct
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
