//
// Created by bhiaibogf on 2021/11/5.
//

#ifndef SKETCHFAB_SPOT_LIGHT_H
#define SKETCHFAB_SPOT_LIGHT_H

#include "light.h"

class SpotLight : public Light {
public:
    SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float cut_off, float outer_cut_off);

    ~SpotLight() override = default;

    void SetShader(Shader &shader) override;

private:
    glm::vec3 position_, direction_;
    float cut_off_, outer_cut_off_;

};


#endif //SKETCHFAB_SPOT_LIGHT_H
