//
// Created by bhiaibogf on 2021/11/10.
//

#include "base_model.h"

BaseModel::BaseModel() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    model_ = glm::mat4(1.0);
}

BaseModel::~BaseModel() {
    // glDeleteVertexArrays(1, &vao_);
    // glDeleteBuffers(1, &vbo_);
}

void BaseModel::Rotate(float degrees, glm::vec3 v) {
    model_ = glm::rotate(glm::mat4(1.f), glm::radians(degrees), v) * model_;
}

void BaseModel::Scale(glm::vec3 v) {
    model_ = glm::scale(glm::mat4(1.f), v) * model_;
}

void BaseModel::Translate(glm::vec3 v) {
    model_ = glm::translate(glm::mat4(1.f), v) * model_;
}

void BaseModel::Draw(const Shader &shader) const {
    shader.setMat4("uModel", model_);
}
