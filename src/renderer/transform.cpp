//
// Created by bhiaibogf on 2021/11/11.
//

#include "transform.h"

Transform::Transform() {
    model_transform_ = model_ = glm::mat4(1.f);
    view_ = glm::mat4(1.f);
    projection_ = glm::mat4(1.f);
}

Transform::Transform(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
        : model_(model), view_(view), projection_(projection), model_transform_(model) {
}

void Transform::Rotate(float degrees, glm::vec3 v) {
    model_ = glm::rotate(model_, glm::radians(degrees), v);
}

void Transform::Scale(glm::vec3 v) {
    model_ = glm::scale(model_, v);
}

void Transform::Translate(glm::vec3 v) {
    model_ = glm::translate(model_, v);
}

void Transform::Update() {
    model_transform_ = model_;
}

void Transform::Update(glm::mat4 transform) {
    model_transform_ = transform * model_;
}
