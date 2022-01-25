//
// Created by bhiaibogf on 2021/11/5.
//

#ifndef SKETCHFAB_POINT_LIGHT_H
#define SKETCHFAB_POINT_LIGHT_H

#include "light.h"
#include "../model/cube.h"

class PointLight : public Light {
public:
    PointLight(glm::vec3 color, glm::vec3 position);

    ~PointLight() override;

    void SetShader(Shader &shader) override;

    void SetShader(Shader &shader, int idx);

    void Draw(Shader &shader);

    bool is_orthographic() override;

protected:
    glm::vec3 position_;

private:
    static int idx_;
    Cube cube_;

};


#endif //SKETCHFAB_POINT_LIGHT_H
