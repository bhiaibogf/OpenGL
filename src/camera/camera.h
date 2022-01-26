//
// Created by bhiaibogf on 2021/10/30.
//

#ifndef SKETCHFAB_CAMERA_H
#define SKETCHFAB_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "../utils/shader.h"

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum CameraMovement {
        kForward,
        kBackward,
        kLeft,
        kRight
    };

    // constructor with vectors
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float yaw = kYaw, float pitch = kPinch);

    // constructor with scalar values
    Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);

    auto &position() const { return position_; }

    auto &front() const { return front_; }

    auto &up() const { return up_; }

    auto &right() const { return right_; }

    auto &world_up() const { return world_up_; }

    auto &view() const { return view_; }

    auto &projection() const { return projection_; }

    void SetShader(const Shader &shader) const;

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

private:
    // Default camera values
    constexpr static const float kYaw = -90.0f;
    constexpr static const float kPinch = 0.0f;
    constexpr static const float kZoom = 45.0f;

    constexpr static const float kSpeed = 2.5f;
    constexpr static const float kSensitivity = 0.1f;

    const unsigned int kScrWidth = 800;
    const unsigned int kScrHeight = 600;

    // camera Attributes
    glm::vec3 position_;
    glm::vec3 world_up_;
    glm::vec3 front_, up_, right_;

    glm::mat4 view_;
    glm::mat4 projection_;

    // euler Angles
    float yaw_;
    float pitch_;

    // camera options
    float zoom_;
    float movement_speed_;
    float mouse_sensitivity_;

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    auto GetViewMatrix() const;

    auto GetProjectionMatrix() const;

    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();

    void UpdateMatrix();

};

#endif //SKETCHFAB_CAMERA_H
