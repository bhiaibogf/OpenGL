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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 directional_light_proj_view;
uniform mat4 point_light_proj_view[4];
uniform mat4 spot_light_proj_view;

void main() {
    vs_out.WorldPos = vec3(model * vec4(aPos, 1.0));
    vs_out.FragPosDirLightSpace = directional_light_proj_view * vec4(vs_out.WorldPos, 1.0);
    for (int i=0;i<4;i++){
        vs_out.FragPosPointLightSpace[i] = point_light_proj_view[i] * vec4(vs_out.WorldPos, 1.0);
    }
    vs_out.FragPosSpotLightSpace = spot_light_proj_view * vec4(vs_out.WorldPos, 1.0);

    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;

    vs_out.TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(vs_out.WorldPos, 1.0);
}
