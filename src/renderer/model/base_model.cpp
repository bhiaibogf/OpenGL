//
// Created by bhiaibogf on 2021/11/10.
//

#include "base_model.h"

BaseModel::BaseModel() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
}

BaseModel::~BaseModel() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}
