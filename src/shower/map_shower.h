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

    void Show(unsigned int map, int idx);

private:
    Shader rgb_shader_ = Shader("shader/quad.vs", "shader/show_map.fs");
    Shader a_shader_ = Shader("shader/quad.vs", "shader/show_map_channel.fs");

};


#endif //SKETCHFAB_MAP_SHOWER_H
