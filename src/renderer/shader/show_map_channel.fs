#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform int idx;
uniform sampler2D map;

void main() {
    vec3 color = vec3(texture(map, TexCoords)[idx]);
    FragColor = vec4(vec3(color), 1.0);
}