//
// Created by bhiaibogf on 2021/11/11.
//

#ifndef SKETCHFAB_TRANSFORM_H
#define SKETCHFAB_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform {
public:
    Transform();

    Transform(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);

    ~Transform() = default;

    void set_model(const glm::mat4 &model) {
        model_ = model;
    }

    void set_view(const glm::mat4 &view) {
        view_ = view;
    }

    void set_projection(const glm::mat4 &projection) {
        projection_ = projection;
    }

    const glm::mat4 &get_model() const {
        return model_transform_;
    }

    const glm::mat4 &get_view() const {
        return view_;
    }

    const glm::mat4 &get_projection() const {
        return projection_;
    }

    void Rotate(float degrees, glm::vec3 v);

    void Scale(glm::vec3 v);

    void Translate(glm::vec3 v);

    void Update(glm::mat4 transform);

private:
    glm::mat4 model_, view_, projection_;
    glm::mat4 model_transform_;

};

#endif //SKETCHFAB_TRANSFORM_H
