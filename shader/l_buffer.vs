#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out Varying{
    vec4 FragPosDirLightSpace;
    vec4 FragPosPointLightSpace[4];
    vec4 FragPosSpotLightSpace;
} vs_out;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform mat4 uDirectionalLightProjView;
uniform mat4 uPointLightProjView[4];
uniform mat4 uSpotLightProjView;

void main() {
    vec4 world_pos = uModel * vec4(aPos, 1.0);

    vs_out.FragPosDirLightSpace = uDirectionalLightProjView * world_pos;
    for (int i=0;i<4;i++){
        vs_out.FragPosPointLightSpace[i] = uPointLightProjView[i] * world_pos;
    }
    vs_out.FragPosSpotLightSpace = uSpotLightProjView * world_pos;

    gl_Position = uProjection * uView * world_pos;
}
