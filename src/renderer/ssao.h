//
// Created by bhiaibogf on 2021/11/13.
//

#ifndef SKETCHFAB_SSAO_H
#define SKETCHFAB_SSAO_H

#include <random>

#include <learnopengl/shader.h>

#include "model/quad.h"

class SSAO {
public:
    SSAO(unsigned int width, unsigned int height);

    ~SSAO();

    Shader &get_shader() { return ssao_shader_; }

    unsigned int get_ssao() const { return ssao_map_; }

    unsigned int get_ssao_blur() const { return blur_map_; }

    void Generate();

    void Blur();

private:
    unsigned int ssao_fbo_, blur_fbo_;
    unsigned int ssao_map_, blur_map_;

    unsigned int width_, height_;

    Quad quad_;
    Shader ssao_shader_ = Shader("shader/quad.vs", "shader/ssao.fs");
    Shader blur_shader_ = Shader("shader/quad.vs", "shader/blur.fs");

};

class Sampler {
public:
    static std::vector<glm::vec3> GenerateKernel();

    static unsigned int GenerateNoises();

private:
    static std::uniform_real_distribution<float> random_float_;
    static std::default_random_engine generator_;
    static std::vector<glm::vec3> kernel;
    static unsigned int noise_map_;

};

#endif //SKETCHFAB_SSAO_H
