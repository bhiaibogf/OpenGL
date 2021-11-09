//
// Created by bhiaibogf on 2021/11/8.
//

#ifndef SKETCHFAB_G_BUFFER_H
#define SKETCHFAB_G_BUFFER_H

#include <learnopengl/shader.h>

class GBuffer {
public:
    GBuffer(unsigned int width, unsigned int height) : width_(width), height_(height) {
        Init();
    }

    ~GBuffer() = default;

    void Draw();

    Shader &get_shader() { return shader; }

    unsigned int get_fbo() {
        return fbo_;
    }

    unsigned int get_g_position() const {
        return g_position_;
    }

    unsigned int get_g_normal() const {
        return g_normal_;
    }

    unsigned int get_g_albedo_roughness() const {
        return g_albedo_roughness_;
    }

    unsigned int get_depth_rbo() const {
        return depth_rbo_;
    }

private:
    unsigned int width_, height_;
    unsigned int fbo_;
    unsigned int g_position_, g_normal_, g_albedo_roughness_;
    unsigned int depth_rbo_;

    Shader shader = Shader("shader/g_buffer.vs", "shader/g_buffer.fs");

    void Init();

};

#endif //SKETCHFAB_G_BUFFER_H
