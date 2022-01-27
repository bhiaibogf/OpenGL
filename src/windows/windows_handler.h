//
// Created by bhiaibogf on 2022/1/25.
//

#ifndef SKETCHFAB_WINDOWS_HANDLER_H
#define SKETCHFAB_WINDOWS_HANDLER_H


#include "../camera/camera.h"
#include "config.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

class WindowsHandler {
public:
    WindowsHandler(int width, int height, Camera *camera);

    ~WindowsHandler();

    static auto yaw() { return yaw_; }

    static auto pitch() { return pitch_; }

    static auto offset_x() { return offset_x_; }

    static auto offset_y() { return offset_y_; }

    static bool ShouldClose();

    static void Clear();

    static void ShowImGui(Config *config);

    static void ProcessInput();

    static void SwapBuffer();

private:
    static GLFWwindow *window_;
    static Camera *camera_;

    static int width_, height_;

    static bool left_button_down_, right_button_down_, middle_button_down_;

    static float last_x_, last_y_;
    static float yaw_, pitch_;
    static float offset_x_, offset_y_;

    static float delta_time_, last_time_;

    static void Init();

    static void InitGlfw();

    static void InitGlad();

    static void InitImGui();

    static void UpdateTime();

    static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);

    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

};


#endif //SKETCHFAB_WINDOWS_HANDLER_H
