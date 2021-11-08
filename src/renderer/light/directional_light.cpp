//
// Created by bhiaibogf on 2021/11/5.
//

#include "directional_light.h"

DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 direction) : Light(color), direction_(direction) {
    far_plane_ += 5.f;

    glm::mat4 light_projection, light_view;
    light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane_, far_plane_);
    light_view = glm::lookAt(-direction_ * 5.f, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    light_space_matrix_ = light_projection * light_view;
}

void DirectionalLight::SetShader(Shader &shader) {
    shader.use();
    shader.setVec3("directional_light.color", color_);
    shader.setVec3("directional_light.direction", direction_);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    shader.setInt("directional_light.shadow_map", 10);
    shader.setMat4("dirLightSpaceMatrix", light_space_matrix_);
}

void DirectionalLight::SetDepthShader(Shader &shader) {
    Light::SetDepthShader(shader);
}
