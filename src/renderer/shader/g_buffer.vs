#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out Varying{
    vec4 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    vs_out.WorldPos = uModel * vec4(aPos, 1.0);
    vs_out.Normal = transpose(inverse(mat3(uModel))) * aNormal;
    vs_out.TexCoords = aTexCoords;

    gl_Position = uProjection * uView * vs_out.WorldPos;
}
