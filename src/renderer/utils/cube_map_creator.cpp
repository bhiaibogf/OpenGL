//
// Created by bhiaibogf on 2021/11/13.
//

#include "cube_map_creator.h"


CubeMapCreator::CubeMapCreator() {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glGenRenderbuffers(1, &depth_rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, kCaptureSize, kCaptureSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo_);

    // pbr: setup cube map to render to and attach to framebuffer
    glGenTextures(1, &map_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, map_);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, kCaptureSize, kCaptureSize, 0, GL_RGB, GL_FLOAT,
                     nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

CubeMapCreator::~CubeMapCreator() {

}

unsigned int CubeMapCreator::ConvertFromSkyBox() {
    return 0;
}

unsigned int CubeMapCreator::ConvertFromEquirectangularMap(std::string path) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, components;
    float *data = stbi_loadf(path.c_str(), &width, &height, &components, 0);
    unsigned int hdr_texture;
    if (data) {
        glGenTextures(1, &hdr_texture);
        glBindTexture(GL_TEXTURE_2D, hdr_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    shader_.use();

    shader_.setMat4("uProjection", kCaptureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr_texture);
    shader_.setInt("uEquirectangularMap", 0);

    // configure the viewport to the capture dimensions.
    glViewport(0, 0, kCaptureSize, kCaptureSize);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    for (unsigned int i = 0; i < 6; ++i) {
        shader_.setMat4("uView", kCaptureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, map_, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Cube().Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, map_);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return map_;
}
