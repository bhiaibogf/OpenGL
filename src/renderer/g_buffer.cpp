//
// Created by bhiaibogf on 2021/11/8.
//

#include "g_buffer.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GBuffer::GBuffer(unsigned int width, unsigned int height) : width_(width), height_(height) {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    // position color buffer
    AddBuffer(g_position_, 0);

    AddBuffer(g_pos_dir_light_, 1);
    for (int i = 0; i < 4; i++) {
        AddBuffer(g_pos_point_light_[i], 2 + i);
    }
    // AddBuffer(g_pos_spot_light_, 6);

    // normal color buffer
    AddBuffer(g_normal_, 6);

    // color + roughness color buffer
    AddBuffer(g_albedo_roughness_, 7);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[8];
    for (int i = 0; i < 8; i++) {
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glDrawBuffers(8, attachments);

    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &depth_rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo_);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader_.use();
}

void GBuffer::AddBuffer(unsigned int &map, int idx) const {
    glGenTextures(1, &map);
    glBindTexture(GL_TEXTURE_2D, map);
    if (idx == 0 || idx == 6) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width_, height_, 0, GL_RGB, GL_FLOAT, nullptr);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx, GL_TEXTURE_2D, map, 0);
}

void GBuffer::SetGBuffer(Shader &shader) {
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_position_);
    shader.setInt("gPosition", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_pos_dir_light_);
    shader.setInt("gFragPosDirLightSpace", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_pos_point_light_[0]);
    shader.setInt("gFragPosPointLightSpace0", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, g_pos_point_light_[1]);
    shader.setInt("gFragPosPointLightSpace1", 3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, g_pos_point_light_[2]);
    shader.setInt("gFragPosPointLightSpace2", 4);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, g_pos_point_light_[3]);
    shader.setInt("gFragPosPointLightSpace3", 5);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, g_normal_);
    shader.setInt("gNormal", 6);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, g_albedo_roughness_);
    shader.setInt("gAlbedoRoughness", 7);
}
