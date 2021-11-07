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

    unsigned int GetFBO() const { return depth_map_fbo_; }

    unsigned int GetDepthMap() const { return depth_map_; }


private:
    glm::vec3 direction_;

    glm::mat4 light_space_matrix;

    unsigned int depth_map_fbo_;
    unsigned int depth_map_;

};


#endif //SKETCHFAB_DIRECTIONAL_LIGHT_H
