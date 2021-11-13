//
// Created by bhiaibogf on 2021/11/9.
//

#include "map_shower.h"

void MapShower::Show(unsigned int map) {
    rgb_shader_.use();
    Shower::Show(map);
}

void MapShower::Show(unsigned int map, int idx) {
    a_shader_.use();
    a_shader_.setInt("idx", idx);
    Shower::Show(map);
}
