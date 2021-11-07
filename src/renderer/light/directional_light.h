//
// Created by bhiaibogf on 2021/11/5.
//

#ifndef SKETCHFAB_DIRECTIONAL_LIGHT_H
#define SKETCHFAB_DIRECTIONAL_LIGHT_H

#include "light.h"

class DirectionalLight : public Light {
public:
    DirectionalLight(glm::vec3 color, glm::vec3 direction);

    ~DirectionalLight() override = default;

    void SetShader(Shader &shader) override;

    void SetDepthShader(Shader &shader) override;

private:
    glm::vec3 direction_;

};


#endif //SKETCHFAB_DIRECTIONAL_LIGHT_H
