//
// Created by bhiaibogf on 2021/11/8.
//

#ifndef SKETCHFAB_SHOWER_H
#define SKETCHFAB_SHOWER_H

#include <learnopengl/shader.h>

#include "../model/quad.h"

class Shower {
public:
    Shower() = default;

    ~Shower() = default;

    virtual void Show(unsigned int map);

private:
    Quad quad_;

};

#endif //SKETCHFAB_SHOWER_H
