//
// Created by bhiaibogf on 2021/11/8.
//

#include "g_buffer.h"

#include <iostream>

GBuffer::GBuffer(unsigned int width, unsigned int height) : width_(width), height_(height) {
    glGenFramebuffers(1, &g_fbo_);

    glBindFramebuffer(GL_FRAMEBUFFER, g_fbo_);

    AddBuffer(g_position_, 0);
    AddBuffer(g_normal_id_, 1);
    AddBuffer(g_albedo_, 2);
    AddBuffer(g_ao_metallic_roughness_, 3);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int g_attachments[kGBufferNum];
    for (int i = 0; i < kGBufferNum; i++) {
        g_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glDrawBuffers(kGBufferNum, g_attachments);

    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &depth_rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo_);

    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!" << std::endl;
    }

    // light buffer
    glGenFramebuffers(1, &l_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, l_fbo_);

    AddBuffer(g_pos_dir_light_, 0);
    for (int i = 0; i < 4; i++) {
        AddBuffer(g_pos_point_light_[i], 1 + i);
    }
    AddBuffer(g_pos_spot_light_, 5);

    unsigned int l_attachments[kLBufferNum];
    for (int i = 0; i < kLBufferNum; i++) {
        l_attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glDrawBuffers(kLBufferNum, l_attachments);

    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo_);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::BindGBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, g_fbo_);
    glClearColor(0.8f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    g_shader_.use();
}

void GBuffer::BindLBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, l_fbo_);
    glClearColor(0.f, 0.8f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDepthFunc(GL_LEQUAL);

    l_shader_.use();
}

void GBuffer::UnbindLBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthFunc(GL_LESS);
}

void GBuffer::AddBuffer(unsigned int &map, int idx) const {
    glGenTextures(1, &map);
    glBindTexture(GL_TEXTURE_2D, map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
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
    glBindTexture(GL_TEXTURE_2D, g_normal_id_);
    shader.setInt("gNormal", 6);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, g_albedo_);
    shader.setInt("gAlbedoRoughness", 7);

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, g_ao_metallic_roughness_);
    shader.setInt("gId", 8);
}
