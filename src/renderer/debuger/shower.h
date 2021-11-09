//
// Created by bhiaibogf on 2021/11/8.
//

#ifndef SKETCHFAB_SHOWER_H
#define SKETCHFAB_SHOWER_H

#include <learnopengl/shader.h>

class Shower {
public:
    Shower();

    ~Shower() = default;

    virtual void Show(unsigned int map);

private:
    unsigned int quad_vao_;
    unsigned int quad_vbo_;

    constexpr static const float kQuadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
};

#endif //SKETCHFAB_SHOWER_H
