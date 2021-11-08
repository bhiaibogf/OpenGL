//
// Created by bhiaibogf on 2021/11/8.
//

#ifndef SKETCHFAB_DEPTH_SHOWER_H
#define SKETCHFAB_DEPTH_SHOWER_H

#include <learnopengl/shader.h>
#include "light/light.h"

class DepthShower {
public:
    DepthShower();

    ~DepthShower() = default;

    void Draw(Light &light);

private:
    unsigned int quad_vao_ = 0;
    unsigned int quad_vbo_;

    Shader orthographic_shader = Shader("shader/quad.vs", "shader/orthographic_depth.fs");
    Shader perspective_shader = Shader("shader/quad.vs", "shader/perspective_depth.fs");

    void RenderQuad();

};

#endif //SKETCHFAB_DEPTH_SHOWER_H
