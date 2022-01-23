//
// Created by bhiaibogf on 2021/10/30.
//

#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : position_(position), world_up_(up), yaw_(yaw), pitch_(pitch),
          front_(glm::vec3(0.0f, 0.0f, -1.0f)),
          movement_speed_(kSpeed), mouse_sensitivity_(kSensitivity), zoom_(kZoom) {
    UpdateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : position_(glm::vec3(posX, posY, posZ)), world_up_(glm::vec3(upX, upY, upZ)), yaw_(yaw), pitch_(pitch),
          front_(glm::vec3(0.0f, 0.0f, -1.0f)),
          movement_speed_(kSpeed), mouse_sensitivity_(kSensitivity), zoom_(kZoom) {
    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = movement_speed_ * deltaTime;
    if (direction == kForward)
        position_ += front_ * velocity;
    if (direction == kBackward)
        position_ -= front_ * velocity;
    if (direction == kLeft)
        position_ -= right_ * velocity;
    if (direction == kRight)
        position_ += right_ * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch_ > 89.0f)

            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    zoom_ -= (float) yoffset;
    if (zoom_ < 1.0f)
        zoom_ = 1.0f;
    if (zoom_ > 90.0f)
        zoom_ = 45.0f;
}
