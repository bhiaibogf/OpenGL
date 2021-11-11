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

uniform int id;

// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(normal_map, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.WorldPos);
    vec3 Q2  = dFdy(fs_in.WorldPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main() {
    gPosition = fs_in.WorldPos;

    gFragPosDirLightSpace = fs_in.FragPosDirLightSpace;
    for (int i=0;i<4;i++){
        gFragPosPointLightSpace[i] = fs_in.FragPosPointLightSpace[i];
    }
    //    gFragPosSpotLightSpace = fs_in.FragPosSpotLightSpace;

    if (id==0){
        gNormal = normalize(getNormalFromMap());

        gAlbedoRoughness.rgb = texture(albedo_map, fs_in.TexCoords).rgb;
        gAlbedoRoughness.a = texture(ao_map, fs_in.TexCoords).r;
    } else if (id==1) {
        gNormal = vec3(0, 1.0, 0);

        gAlbedoRoughness.rgb = vec3(0.3);
        gAlbedoRoughness.a = 1.0;
    }
}