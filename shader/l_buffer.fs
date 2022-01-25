#version 330 core
layout (location = 0) out vec4 gFragPosDirLightSpace;
layout (location = 1) out vec4 gFragPosPointLightSpace[4];
layout (location = 5) out vec4 gFragPosSpotLightSpace;

in Varying{
    vec4 FragPosDirLightSpace;
    vec4 FragPosPointLightSpace[4];
    vec4 FragPosSpotLightSpace;
} fs_in;

void main() {
    gFragPosDirLightSpace = fs_in.FragPosDirLightSpace;
    for (int i=0;i<4;i++){
        gFragPosPointLightSpace[i] = fs_in.FragPosPointLightSpace[i];
    }
    gFragPosSpotLightSpace = fs_in.FragPosSpotLightSpace;
}