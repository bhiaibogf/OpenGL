//
// Created by bhiaibogf on 2021/11/5.
//

#include "point_light.h"

int PointLight::idx_ = -1;

PointLight::PointLight(glm::vec3 color, glm::vec3 position) : Light(color), position_(position) {
    idx_++;

    glm::mat4 light_projection, light_view;
    light_projection = glm::perspective(glm::radians(45.0f), (GLfloat) kShadowWidth / (GLfloat) kShadowHeight,
                                        near_plane_, far_plane_);
    light_view = glm::lookAt(position_, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    light_space_matrix_ = light_projection * light_view;
}

PointLight::~PointLight() {
    idx_--;
}

void PointLight::SetShader(Shader &shader) {
    shader.use();

    shader.setVec3("point_light[" + std::to_string(idx_) + "].color", color_);
    shader.setVec3("point_light[" + std::to_string(idx_) + "].position", position_);
}

void PointLight::SetShader(Shader &shader, int idx) {
    shader.use();
    shader.setVec3("point_light[" + std::to_string(idx) + "].color", color_);
    shader.setVec3("point_light[" + std::to_string(idx) + "].position", position_);
    glActiveTexture(GL_TEXTURE11 + idx);
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    shader.setInt("point_light[" + std::to_string(idx) + "].shadow_map", 11 + idx);
    shader.setMat4("pointLightSpaceMatrix[" + std::to_string(idx) + "]", light_space_matrix_);
}

void PointLight::SetDepthShader(Shader &shader) {
    Light::SetDepthShader(shader);
}
