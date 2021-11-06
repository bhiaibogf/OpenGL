//
// Created by bhiaibogf on 2021/11/5.
//

#include "point_light.h"

int PointLight::idx_ = -1;

PointLight::PointLight(glm::vec3 color, glm::vec3 position) : Light(color), position_(position) {
    idx_++;
}

PointLight::~PointLight() {
    idx_--;
}

void PointLight::SetShader(Shader &shader) {
    shader.setVec3("point_light[" + std::to_string(idx_) + "].color", color_);
    shader.setVec3("point_light[" + std::to_string(idx_) + "].position", position_);
}

void PointLight::SetShader(Shader &shader, int idx) {
    shader.setVec3("point_light[" + std::to_string(idx) + "].color", color_);
    shader.setVec3("point_light[" + std::to_string(idx) + "].position", position_);
}
