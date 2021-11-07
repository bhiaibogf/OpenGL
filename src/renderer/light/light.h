//
// Created by bhiaibogf on 2021/11/5.
//

#ifndef SKETCHFAB_LIGHT_H
#define SKETCHFAB_LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>

class Light {
public:
    explicit Light(glm::vec3 color);

    virtual ~Light() = default;

    virtual void SetShader(Shader &shader);

    virtual void SetDepthShader(Shader &shader);

    unsigned int GetFBO() const { return depth_map_fbo_; }

    unsigned int GetDepthMap() const { return depth_map_; }

    float GetZNear() const { return near_plane_; }

    float GetZFar() const { return far_plane_; }

protected:
    static const unsigned int kShadowWidth = 1024, kShadowHeight = 1024;

    glm::vec3 color_;

    float near_plane_ = 0.1f, far_plane_ = 10.f;
    glm::mat4 light_space_matrix_;

    unsigned int depth_map_fbo_;
    unsigned int depth_map_;

};


#endif //SKETCHFAB_LIGHT_H
