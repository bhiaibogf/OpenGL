//
// Created by bhiaibogf on 2021/11/8.
//

#ifndef SKETCHFAB_DEPTH_SHOWER_H
#define SKETCHFAB_DEPTH_SHOWER_H

#include "shower.h"
#include "../light/light.h"

class DepthShower : public Shower {
public:
    DepthShower() = default;

    ~DepthShower() = default;

    void Show(Light &light);

private:
    Shader orthographic_shader = Shader("shader/quad.vs", "shader/orthographic_depth.fs");
    Shader perspective_shader = Shader("shader/quad.vs", "shader/perspective_depth.fs");

};

#endif //SKETCHFAB_DEPTH_SHOWER_H
