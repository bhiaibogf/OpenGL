#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vWorldPos;

void main() {
    vWorldPos = aPos;

    mat4 rotate_view = mat4(mat3(uView));
    vec4 clip_pos = uProjection * rotate_view * vec4(vWorldPos, 1.0);

    gl_Position = clip_pos.xyww;
}