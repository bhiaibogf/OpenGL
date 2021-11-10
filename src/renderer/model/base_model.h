//
// Created by bhiaibogf on 2021/11/10.
//

#ifndef SKETCHFAB_BASE_MODEL_H
#define SKETCHFAB_BASE_MODEL_H

#include <glad/glad.h>

#include <learnopengl/shader.h>

class BaseModel {
public:
    BaseModel();

    ~BaseModel();

    virtual void Draw() = 0;

protected:
    unsigned int vao_;
    unsigned int vbo_;

};

#endif //SKETCHFAB_BASE_MODEL_H
