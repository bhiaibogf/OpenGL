#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormalId;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gAoMetallicRoughness;

in Varying{
    vec4 WorldPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

// material parameters
uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uMetallicMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uAoMap;

uniform int uId;

vec3 getNormalFromMap() {
    vec3 tangent_normal = texture(uNormalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 world_position = fs_in.WorldPos.xyz / fs_in.WorldPos.w;
    vec3 Q1  = dFdx(world_position);
    vec3 Q2  = dFdy(world_position);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangent_normal);
}

void main() {
    gPosition = fs_in.WorldPos;
    if (uId==1){
        gNormalId.xyz = normalize(getNormalFromMap());
        gNormalId.w = 0.1;

        gAlbedo.rgb = texture(uAlbedoMap, fs_in.TexCoords).rgb;
        gAoMetallicRoughness.r = texture(uAoMap, fs_in.TexCoords).r;
    } else if (uId==2) {
        gNormalId.xyz = vec3(0, 1.0, 0);
        gNormalId.w = 0.2;

        gAlbedo.rgb = vec3(0.3);
        gAoMetallicRoughness.r = 1.0;
    } else if (uId==3){
        gNormalId.xyz = normalize(vec3(0, -1, 1));
        gNormalId.w = 0.3;

        gAlbedo.rgb = vec3(0.0);
        gAoMetallicRoughness.r = 0.0;
    }
}