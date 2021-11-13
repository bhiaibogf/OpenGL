#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vWorldPos;

uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    vWorldPos = aPos;
    gl_Position =  uProjection * uView * vec4(vWorldPos, 1.0);
}