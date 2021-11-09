//
// Created by bhiaibogf on 2021/11/8.
//

#include "depth_shower.h"

void DepthShower::Show(Light &light) {
    if (light.is_orthographic()) {
        orthographic_shader.use();
    } else {
        perspective_shader.use();
        perspective_shader.setFloat("near_plane", light.GetZNear());
        perspective_shader.setFloat("far_plane", light.GetZFar());
    }
    Shower::Show(light.GetDepthMap());
}
