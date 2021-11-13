#version 330 core
out vec4 FragColor;

in vec3 vWorldPos;

uniform samplerCube uEnvironmentMap;

const float kPi = 3.14159265359;

void main() {
    vec3 normal = normalize(vWorldPos);

    vec3 irradiance = vec3(0.0);

    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up         = normalize(cross(normal, right));

    float delta = 0.025;
    float cnt = 0.0f;
    for (float phi = 0.0; phi < 2.0 * kPi; phi += delta) {
        for (float theta = 0.0; theta < 0.5 * kPi; theta += delta) {
            // spherical to cartesian (in tangent space)
            vec3 sample_tangent = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sample_vec = sample_tangent.x * right + sample_tangent.y * up + sample_tangent.z * normal;

            irradiance += texture(uEnvironmentMap, sample_vec).rgb * cos(theta) * sin(theta);
            cnt++;
        }
    }
    irradiance = kPi * irradiance / cnt;

    FragColor = vec4(irradiance, 1.0);
}
