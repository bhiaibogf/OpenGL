//
// Created by bhiaibogf on 2021/11/10.
//

#ifndef SKETCHFAB_QUAD_H
#define SKETCHFAB_QUAD_H

#include "base_model.h"

class Quad : public BaseModel {
public:
    Quad();

    ~Quad() = default;

    void Draw() const override;

private:
    constexpr static const float kQuadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

};

#endif //SKETCHFAB_QUAD_H
