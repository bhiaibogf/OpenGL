//
// Created by bhiaibogf on 2021/11/5.
//

#ifndef SKETCHFAB_POINT_LIGHT_H
#define SKETCHFAB_POINT_LIGHT_H

#include "light.h"

class PointLight : public Light {
public:
    PointLight(glm::vec3 color, glm::vec3 position);

    ~PointLight() override;

    void SetShader(Shader &shader) override;

    void SetShader(Shader &shader, int idx);

    void SetDepthShader(Shader &shader) override;

private:
    static int idx_;
    glm::vec3 position_;

};


#endif //SKETCHFAB_POINT_LIGHT_H