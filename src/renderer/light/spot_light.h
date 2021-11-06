//
// Created by bhiaibogf on 2021/11/5.
//

#ifndef SKETCHFAB_SPOT_LIGHT_H
#define SKETCHFAB_SPOT_LIGHT_H

#include "light.h"

class SpotLight : Light {
public:

private:
    glm::vec3 position_;
    float cut_off_, outer_cut_off_;

};


#endif //SKETCHFAB_SPOT_LIGHT_H
