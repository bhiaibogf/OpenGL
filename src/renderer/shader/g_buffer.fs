#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gFragPosDirLightSpace;
layout (location = 2) out vec4 gFragPosPointLightSpace[4];
//layout (location = 6) out vec4 gFragPosSpotLightSpace;
layout (location = 6) out vec3 gNormal;
layout (location = 7) out vec4 gAlbedoRoughness;

in Varying{
    vec3 WorldPos;
    vec4 FragPosDirLightSpace;
    vec4 FragPosPointLightSpace[4];
    vec4 FragPosSpotLightSpace;

    vec3 Normal;

    vec2 TexCoords;
} fs_in;

// material parameters
uniform sampler2D albedo_map;
uniform sampler2D normal_map;
uniform sampler2D metallic_map;
uniform sampler2D roughness_map;
uniform sampler2D ao_map;

void main() {
    gPosition = fs_in.WorldPos;

    gFragPosDirLightSpace = fs_in.FragPosDirLightSpace;
    for (int i=0;i<4;i++){
        gFragPosPointLightSpace[i] = fs_in.FragPosPointLightSpace[i];
    }
    //    gFragPosSpotLightSpace = fs_in.FragPosSpotLightSpace;

    gNormal = normalize(fs_in.Normal);

    gAlbedoRoughness.rgb = texture(albedo_map, fs_in.TexCoords).rgb;
    gAlbedoRoughness.a = texture(roughness_map, fs_in.TexCoords).r;
}