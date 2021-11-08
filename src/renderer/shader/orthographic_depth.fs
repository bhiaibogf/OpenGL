#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depth_map;

void main() {
    float depth = texture(depth_map, TexCoords).r;
    FragColor = vec4(vec3(depth), 1.0);
}