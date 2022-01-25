//
// Created by bhiaibogf on 2022/1/25.
//

#include "windows_handler.h"

Camera *WindowsHandler::camera_ = nullptr;
GLFWwindow *WindowsHandler::window_ = nullptr;

float WindowsHandler::last_x_ = 0.f;
float WindowsHandler::last_y_ = 0.f;

float WindowsHandler::pitch_ = 0.f;
float WindowsHandler::yaw_ = 0.f;
float WindowsHandler::offset_x_ = 0.f;
float WindowsHandler::offset_y_ = 0.f;

float WindowsHandler::delta_time_ = 0.f;
float WindowsHandler::last_time_ = 0.f;

bool WindowsHandler::left_button_down_ = false;
bool WindowsHandler::right_button_down_ = false;
bool WindowsHandler::middle_button_down_ = false;

WindowsHandler::WindowsHandler(int width, int height, Camera *camera) {
    Init(width, height);
    camera_ = camera;

    last_x_ = width / 2.f;
    last_y_ = height / 2.f;
}

WindowsHandler::~WindowsHandler() {
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
}

void WindowsHandler::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void WindowsHandler::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (GLFW_PRESS == action) {
                left_button_down_ = true;
                last_x_ = xpos;
                last_y_ = ypos;
            } else if (GLFW_RELEASE == action) {
                left_button_down_ = false;
            }
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (GLFW_PRESS == action) {
                right_button_down_ = true;
                last_x_ = xpos;
                last_y_ = ypos;
            } else if (GLFW_RELEASE == action) {
                right_button_down_ = false;
            }
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            if (GLFW_PRESS == action) {
                middle_button_down_ = true;
                last_x_ = xpos;
                last_y_ = ypos;
            } else if (GLFW_RELEASE == action) {
                middle_button_down_ = false;
            }
            break;
        default:
            assert("unsupported button");
    }
}

void WindowsHandler::CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    float x_offset = xpos - last_x_;
    float y_offset = last_y_ - ypos; // reversed since y-coordinates go from bottom to top

    last_x_ = xpos;
    last_y_ = ypos;

    if (left_button_down_) {
        yaw_ = x_offset;
        pitch_ = -y_offset;
    }
    if (right_button_down_) {
        offset_x_ = x_offset;
        offset_y_ = y_offset;
    }
    if (middle_button_down_) {
        camera_->ProcessMouseMovement(x_offset, y_offset);
    }
}

void WindowsHandler::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    camera_->ProcessMouseScroll(yoffset);
}

void WindowsHandler::ProcessInput() {
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window_, true);

    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera::kForward, delta_time_);
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera::kBackward, delta_time_);
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera::kLeft, delta_time_);
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
        camera_->ProcessKeyboard(Camera::kRight, delta_time_);
}

void WindowsHandler::Clear() {
    yaw_ = pitch_ = 0;
    offset_x_ = offset_y_ = 0;
}

void WindowsHandler::UpdateTime() {
    float current_time = glfwGetTime();
    delta_time_ = current_time - last_time_;
    last_time_ = current_time;
}

void WindowsHandler::SwapBuffer() {
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(window_);
    glfwPollEvents();
}

bool WindowsHandler::ShouldClose() {
    return glfwWindowShouldClose(window_);
}

void WindowsHandler::Init(int width, int height) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    window_ = glfwCreateWindow(width, height, "sketchfab", nullptr, nullptr);
    assert(("Failed to create GLFW window", window_));

    glfwMakeContextCurrent(window_);

    glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
    glfwSetCursorPosCallback(window_, CursorPosCallback);
    glfwSetScrollCallback(window_, ScrollCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    bool load_glad = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    assert(("Failed to initialize GLAD", load_glad));
}
