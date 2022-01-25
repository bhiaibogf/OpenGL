#version 330 core
out vec4 FragColor;

in vec3 vWorldPos;

uniform sampler2D uEquirectangularMap;

const vec2 kInvAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 dir) {
    vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
    uv *= kInvAtan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(vWorldPos));
    vec3 color = texture(uEquirectangularMap, uv).rgb;

    FragColor = vec4(color, 1.0);
}
