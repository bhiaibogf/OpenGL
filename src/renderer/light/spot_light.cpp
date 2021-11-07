//
// Created by bhiaibogf on 2021/11/5.
//

#include "spot_light.h"

SpotLight::SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float cut_off, float outer_cut_off)
        : Light(color), position_(position), direction_(direction), cut_off_(cut_off), outer_cut_off_(outer_cut_off) {}

void SpotLight::SetShader(Shader &shader) {
    shader.setVec3("spot_light.color", color_);
    shader.setVec3("spot_light.position", position_);
    shader.setVec3("spot_light.direction", direction_);

    shader.setFloat("spot_light.cut_off", cut_off_);
    shader.setFloat("spot_light.outer_cut_off", outer_cut_off_);
}
