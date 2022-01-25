//
// Created by bhiaibogf on 2021/11/8.
//

#ifndef SKETCHFAB_G_BUFFER_H
#define SKETCHFAB_G_BUFFER_H

#include "../utils/shader.h"

class GBuffer {
public:
    GBuffer(unsigned int width, unsigned int height);

    ~GBuffer() = default;

    unsigned int get_fbo() const {
        return l_fbo_;
    }

    unsigned int get_depth_rbo() const {
        return depth_rbo_;
    }

    unsigned int get_g_position() const { return g_position_; }

    unsigned int get_g_normal_id() const { return g_normal_id_; }

    unsigned int get_g_depth() const { return g_depth_; }

    unsigned int get_g_albedo() const { return g_albedo_; }

    unsigned int get_g_pos_dir_light() const { return g_pos_dir_light_; }

    unsigned int get_g_ao_metallic_roughness() const { return g_ao_metallic_roughness_; }

    const unsigned int *get_g_pos_point_light() const { return g_pos_point_light_; }

    unsigned int get_g_pos_spot_light() const { return g_pos_spot_light_; }

    Shader &get_g_shader() { return g_shader_; }

    Shader &get_l_shader() { return l_shader_; }

    void SetGBuffer(Shader &shader);

    void BindGBuffer();

    void BindLBuffer();

    void UnbindLBuffer();

private:
    static const int kGBufferNum = 4;
    static const int kLBufferNum = 6;

    unsigned int width_, height_;

    unsigned int g_fbo_, l_fbo_;

    unsigned int g_position_, g_normal_id_, g_albedo_, g_ao_metallic_roughness_, g_depth_;
    unsigned int g_pos_dir_light_, g_pos_point_light_[4], g_pos_spot_light_;

    unsigned int depth_rbo_;

    Shader g_shader_ = Shader("shader/g_buffer.vs", "shader/g_buffer.fs");
    Shader l_shader_ = Shader("shader/l_buffer.vs", "shader/l_buffer.fs");

    void AddBuffer(unsigned int &map, int idx) const;

};

#endif //SKETCHFAB_G_BUFFER_H
