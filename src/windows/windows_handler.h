//
// Created by bhiaibogf on 2022/1/25.
//

#ifndef SKETCHFAB_WINDOWS_HANDLER_H
#define SKETCHFAB_WINDOWS_HANDLER_H


#include "../camera/camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class WindowsHandler {
public:
    WindowsHandler(int width, int height, Camera *camera);

    ~WindowsHandler();

    static auto yaw() { return yaw_; }

    static auto pitch() { return pitch_; }

    static auto offset_x() { return offset_x_; }

    static auto offset_y() { return offset_y_; }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    // glfw: whenever the mouse moves, this callback is called
    static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    static void ProcessInput();

    static void Clear();

    static void UpdateTime();

    static void SwapBuffer();

    static bool ShouldClose();

private:
    static GLFWwindow *window_;
    static Camera *camera_;

    static bool left_button_down_, right_button_down_, middle_button_down_;

    static float last_x_, last_y_;
    static float yaw_, pitch_;
    static float offset_x_, offset_y_;

    static float delta_time_, last_time_;

    static void Init(int width, int height);
};


#endif //SKETCHFAB_WINDOWS_HANDLER_H
