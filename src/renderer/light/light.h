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
    explicit Light(glm::vec3 color) : color_(color) {}

    virtual ~Light() = default;

    virtual void SetShader(Shader &shader) {
        shader.setVec3("color", color_);
    }

    virtual void SetDepthShader(Shader &shader) {}

protected:
    glm::vec3 color_;

};


#endif //SKETCHFAB_LIGHT_H
