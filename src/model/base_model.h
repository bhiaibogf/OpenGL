//
// Created by bhiaibogf on 2021/11/10.
//

#ifndef SKETCHFAB_BASE_MODEL_H
#define SKETCHFAB_BASE_MODEL_H


#include "../utils/shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class BaseModel {
public:
    BaseModel();

    ~BaseModel();

    void Rotate(float degrees, glm::vec3 v);

    void Scale(glm::vec3 v);

    void Translate(glm::vec3 v);

    virtual void Draw() const = 0;

    virtual void SetModelMatrixAndDraw(const Shader &shader) const;

protected:
    unsigned int vao_;
    unsigned int vbo_;
    glm::mat4 model_;

    void SetModel(const Shader &shader) const;

};

#endif //SKETCHFAB_BASE_MODEL_H
