//
// Created by bhiaibogf on 2021/11/9.
//

#ifndef SKETCHFAB_MAP_SHOWER_H
#define SKETCHFAB_MAP_SHOWER_H

#include "shower.h"

class MapShower : public Shower {
public:
    MapShower() = default;

    ~MapShower() = default;

    void Show(unsigned int map) override;

private:
    Shader rgb_shader = Shader("shader/quad.vs", "shader/show_map.fs");
    Shader a_shader = Shader("shader/quad.vs", "shader/orthographic_depth.fs");

};


#endif //SKETCHFAB_MAP_SHOWER_H
