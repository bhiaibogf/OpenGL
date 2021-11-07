//
// Created by bhiaibogf on 2021/11/5.
//

#include "directional_light.h"

DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 direction) : Light(color), direction_(direction) {
    // configure depth map FBO
    // -----------------------
    glGenFramebuffers(1, &depth_map_fbo_);

    // create depth texture
    const unsigned int kShadowWidth = 1024, kShadowHeight = 1024;
    glGenTextures(1, &depth_map_);
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 kShadowWidth, kShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DirectionalLight::SetShader(Shader &shader) {
    shader.use();
    shader.setVec3("directional_light.color", color_);
    shader.setVec3("directional_light.direction", direction_);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, depth_map_);
    shader.setInt("directional_light.shadow_map", 10);
    shader.setMat4("lightSpaceMatrix", light_space_matrix);
}

void DirectionalLight::SetDepthShader(Shader &shader) {
    glm::mat4 light_projection, light_view;
    float near_plane = 1.0f, far_plane = 7.5f;
    //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    light_view = glm::lookAt(glm::vec3(0.f) - direction_, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    light_space_matrix = light_projection * light_view;
    // render scene from light's point of view
    shader.use();
    shader.setMat4("lightSpaceMatrix", light_space_matrix);
}
