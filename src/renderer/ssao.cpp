//
// Created by bhiaibogf on 2021/11/13.
//

#include "ssao.h"

SSAO::SSAO(unsigned int width, unsigned int height) : width_(width), height_(height) {
    glGenFramebuffers(1, &ssao_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo_);

    glGenTextures(1, &ssao_map_);
    glBindTexture(GL_TEXTURE_2D, ssao_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_map_, 0);

    glGenFramebuffers(1, &blur_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo_);

    glGenTextures(1, &blur_map_);
    glBindTexture(GL_TEXTURE_2D, blur_map_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur_map_, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SSAO::~SSAO() {
    glDeleteFramebuffers(1, &ssao_fbo_);
    glDeleteTextures(1, &ssao_map_);
    glDeleteFramebuffers(1, &blur_fbo_);
    glDeleteTextures(1, &blur_map_);
}

void SSAO::Generate() {
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo_);
    {
        glClear(GL_COLOR_BUFFER_BIT);

        ssao_shader_.use();

        auto ssao_kernel = Sampler::GenerateKernel();
        // Send kernel + rotation
        for (unsigned int i = 0; i < 64; ++i) {
            ssao_shader_.setVec3("uSamples[" + std::to_string(i) + "]", ssao_kernel[i]);
        }

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Sampler::GenerateNoiseMap());
        ssao_shader_.setInt("uNoiseMap", 2);

        quad_.Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::Blur() {
    glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo_);
    {
        glClear(GL_COLOR_BUFFER_BIT);

        blur_shader_.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssao_map_);
        blur_shader_.setInt("uMap", 0);

        quad_.Draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::uniform_real_distribution<float> Sampler::random_float_ = std::uniform_real_distribution<float>(-1.0, 1.0);
std::default_random_engine Sampler::generator_ = std::default_random_engine();

std::vector<glm::vec3> Sampler::kernel_;

// generate sample kernel
std::vector<glm::vec3> Sampler::GenerateKernel() {
    if (kernel_.empty()) {
        for (int i = 0; i < 64; i++) {
            glm::vec3 sample(random_float_(generator_), random_float_(generator_),
                             random_float_(generator_) * 0.5 + 0.5);
            sample = glm::normalize(sample);

            sample *= random_float_(generator_);

            // scale samples s.t. they're more aligned to center of kernel
            float scale = float(i) / 64.0;
            auto lerp = [](float a, float b, float f) -> float {
                return a + f * (b - a);
            };

            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;

            kernel_.push_back(sample);
        }
    }
    return kernel_;
}

unsigned int Sampler::noise_map_ = 0;

// generate noise texture
unsigned int Sampler::GenerateNoiseMap() {
    if (noise_map_ == 0) {
        glGenTextures(1, &noise_map_);
        glBindTexture(GL_TEXTURE_2D, noise_map_);

        std::vector<glm::vec3> noise_list;
        for (int i = 0; i < 25; i++) {
            // rotate around z-axis (in tangent space)
            glm::vec3 noise(random_float_(generator_), random_float_(generator_), 0.0f);
            noise_list.push_back(noise);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 5, 5, 0, GL_RGB, GL_FLOAT, &noise_list[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    return noise_map_;
}
