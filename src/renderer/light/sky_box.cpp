//
// Created by bhiaibogf on 2021/11/13.
//

#include "sky_box.h"

SkyBox::SkyBox(unsigned int map) : map_(map) {

}

void SkyBox::Draw(Camera &camera) {
    // render skybox (render as last to prevent overdraw)
    shader_.use();

    shader_.setMat4("uView", camera.GetViewMatrix());
    shader_.setMat4("uProjection", camera.GetProjectionMatrix());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, map_);
    shader_.setInt("uEnvironmentMap", 0);

    Cube().Draw();
}
