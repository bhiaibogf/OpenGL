//
// Created by bhiaibogf on 2021/11/5.
//

#include "directional_light.h"

DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 direction) : Light(color), direction_(direction) {}

void DirectionalLight::SetShader(Shader &shader) {
    shader.setVec3("directional_light.color", color_);
    shader.setVec3("directional_light.direction", direction_);
}
