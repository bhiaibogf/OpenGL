//
// Created by bhiaibogf on 2021/11/9.
//

#include "map_shower.h"

void MapShower::Show(unsigned int map) {
    rgb_shader.use();
    Shower::Show(map);
}
