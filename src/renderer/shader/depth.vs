#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 light_proj_view;

void main() {
    gl_Position = light_proj_view * model * vec4(aPos, 1.0);
}