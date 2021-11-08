#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VsOut{
    vec3 WorldPos;
    vec4 FragPosDirLightSpace;
    vec4 FragPosPointLightSpace[4];
    vec4 FragPosSpotLightSpace;

    vec3 Normal;

    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 dirLightSpaceMatrix;
uniform mat4 pointLightSpaceMatrix[4];
uniform mat4 spotLightSpaceMatrix;

void main() {
    vs_out.WorldPos = vec3(model * vec4(aPos, 1.0));
    vs_out.FragPosDirLightSpace = dirLightSpaceMatrix * vec4(vs_out.WorldPos, 1.0);
    for (int i=0;i<4;i++){
        vs_out.FragPosPointLightSpace[i] = pointLightSpaceMatrix[i] * vec4(vs_out.WorldPos, 1.0);
    }
    vs_out.FragPosSpotLightSpace = spotLightSpaceMatrix * vec4(vs_out.WorldPos, 1.0);

    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;

    vs_out.TexCoords = aTexCoords;

    gl_Position =  projection * view * vec4(vs_out.WorldPos, 1.0);
}