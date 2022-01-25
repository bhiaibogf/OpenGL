#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D map;

void main() {
    vec3 color = texture(map, TexCoords).rgb;
    FragColor = vec4(vec3(color), 1.0);
}