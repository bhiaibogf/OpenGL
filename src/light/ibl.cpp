//
// Created by bhiaibogf on 2021/11/13.
//

#include "ibl.h"

IBL::IBL(const std::string &path) {
    glGenFramebuffers(1, &fbo_);
    glGenRenderbuffers(1, &rbo_);

    sky_box_map_ = CubeMapCreator().ConvertFromEquirectangularMap(path);
    Prt();
}

IBL::~IBL() {

}

void IBL::Prt() {
    GetIrradianceMap();
    GetPrefilterMap();
    GetLut();
}

void IBL::BindFrameBuffer(unsigned int &map, int size) {
    glGenTextures(1, &map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, map);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
}

void IBL::GetIrradianceMap() {
    // pbr: create an irradiance cube map, and re-scale capture FBO to irradiance scale.
    BindFrameBuffer(irradiance_map_, kIrradianceMapSize);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    irradiance_shader_.use();
    irradiance_shader_.setMat4("uProjection", kCaptureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sky_box_map_);
    irradiance_shader_.setInt("uEnvironmentMap", 0);

    // configure the viewport to the capture dimensions.
    glViewport(0, 0, kIrradianceMapSize, kIrradianceMapSize);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    for (int i = 0; i < 6; i++) {
        irradiance_shader_.setMat4("uView", kCaptureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               irradiance_map_, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube_.Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBL::GetPrefilterMap() {
    // pbr: create a pre-filter cube map, and re-scale capture FBO to pre-filter scale.
    BindFrameBuffer(prefilter_map_, kPrefilterMapSize);
    // be sure to set minification filter to mip_linear
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // generate mipmaps for the cube map so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    prefilter_shader_.use();

    prefilter_shader_.setMat4("uProjection", kCaptureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sky_box_map_);
    prefilter_shader_.setInt("uEnvironmentMap", 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    unsigned int max_mip_levels = 5;
    for (int mip_level = 0; mip_level < max_mip_levels; mip_level++) {
        // resize framebuffer according to mip-level size.
        unsigned int mipmap_size = kPrefilterMapSize * (unsigned int) std::pow(0.5, mip_level);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipmap_size, mipmap_size);
        glViewport(0, 0, mipmap_size, mipmap_size);

        float roughness = (float) mip_level / (float) (max_mip_levels - 1);
        prefilter_shader_.setFloat("uRoughness", roughness);

        for (int i = 0; i < 6; i++) {
            prefilter_shader_.setMat4("uView", kCaptureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                   prefilter_map_, mip_level);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cube_.Draw();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBL::GetLut() {
    // pbr: generate a 2D LUT from the BRDF equations used.
    glGenTextures(1, &brdf_lut_map_);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdf_lut_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, kLutMapSize, kLutMapSize, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, kLutMapSize, kLutMapSize);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf_lut_map_, 0);

    glViewport(0, 0, kLutMapSize, kLutMapSize);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    brdf_shader_.use();
    quad_.Draw();
}

void IBL::SetIblMap(Shader &shader) {
    shader.use();

    glActiveTexture(GL_TEXTURE21);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_map_);
    shader.setInt("uIrradianceMap", 21);

    glActiveTexture(GL_TEXTURE22);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_map_);
    shader.setInt("uPrefilterMap", 22);

    glActiveTexture(GL_TEXTURE23);
    glBindTexture(GL_TEXTURE_2D, brdf_lut_map_);
    shader.setInt("uBrdfLutMap", 23);
}
