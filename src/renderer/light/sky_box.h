//
// Created by bhiaibogf on 2021/11/13.
//

#ifndef SKETCHFAB_SKY_BOX_H
#define SKETCHFAB_SKY_BOX_H

#include "../camera.h"
#include <learnopengl/shader.h>

#include "../model/cube.h"

class SkyBox {
public:
    explicit SkyBox(unsigned int map);

    void Draw(Camera &camera);

private:
    unsigned int map_;
    Shader shader_ = Shader("shader/skybox.vs", "shader/skybox.fs");

};


#endif //SKETCHFAB_SKY_BOX_H
