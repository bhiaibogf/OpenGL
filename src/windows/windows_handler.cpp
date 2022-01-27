//
// Created by bhiaibogf on 2022/1/25.
//

#include "windows_handler.h"

int WindowsHandler::width_ = 800;
int WindowsHandler::height_ = 600;
GLFWwindow *WindowsHandler::window_ = nullptr;

Camera *WindowsHandler::camera_ = nullptr;

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
    width_ = width;
    height_ = height;
    Init();

    camera_ = camera;

    last_x_ = width / 2.f;
    last_y_ = height / 2.f;
}

WindowsHandler::~WindowsHandler() {
    // ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // GLFW
    glfwDestroyWindow(window_);
    glfwTerminate();
}

bool WindowsHandler::ShouldClose() {
    return glfwWindowShouldClose(window_);
}

void WindowsHandler::Clear() {
    yaw_ = pitch_ = 0;
    offset_x_ = offset_y_ = 0;
}

void WindowsHandler::ShowImGui(Config *config) {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("rendering controller");

    ImGui::RadioButton("renderer", (int *) &(config->type), (int) Type::kRenderer);
    ImGui::SameLine();
    ImGui::RadioButton("prt", (int *) &(config->type), (int) Type::kPrt);
    ImGui::RadioButton("l-buffer", (int *) &(config->type), (int) Type::kLBuffer);
    ImGui::SameLine();
    ImGui::RadioButton("g-buffer", (int *) &(config->type), (int) Type::kGBuffer);

    switch (config->type) {
        case Type::kRenderer:
            ImGui::Text("Show scene.\n");

            ImGui::Checkbox("add ambient", &config->ambient);
            ImGui::Checkbox("add direct light", &config->lo);
            ImGui::Checkbox("add ao", &config->ao);
            ImGui::Checkbox("use ao_map", &config->ao_map);
            ImGui::Checkbox("use ibl", &config->ibl);
            break;
        case Type::kPrt:
            ImGui::Text("Show prt.\n");

            ImGui::RadioButton("skybox", (int *) &(config->prt), (int) Prt::kSkybox);
            ImGui::SameLine();
            ImGui::RadioButton("irradiance", (int *) &(config->prt), (int) Prt::kIrradiance);
            ImGui::RadioButton("prefilter", (int *) &(config->prt), (int) Prt::kPrefilter);
            ImGui::SameLine();
            ImGui::RadioButton("lut", (int *) &(config->prt), (int) Prt::kLut);
            break;
        case Type::kLBuffer:
            ImGui::Text("Show l-buffer.\n");

            ImGui::RadioButton("directional light depth", (int *) &(config->l_buf), (int) LBuf::kDir);
            ImGui::SameLine();
            ImGui::RadioButton("point light depth", (int *) &(config->l_buf), (int) LBuf::kPoint);

            ImGui::RadioButton("directional light position", (int *) &(config->l_buf), (int) LBuf::kDirPos);
            ImGui::SameLine();
            ImGui::RadioButton("point light position", (int *) &(config->l_buf), (int) LBuf::kPointPos);

            if (config->l_buf == LBuf::kPoint || config->l_buf == LBuf::kPointPos) {
                ImGui::SliderInt("which point light", &config->light, 0, 3);
            }
            break;
        case Type::kGBuffer:
            ImGui::Text("Show g-buffer.\n");

            ImGui::RadioButton("position", (int *) &(config->g_buf), (int) GBuf::kPos);
            ImGui::SameLine();
            ImGui::RadioButton("albedo", (int *) &(config->g_buf), (int) GBuf::kAlb);
            ImGui::RadioButton("normal", (int *) &(config->g_buf), (int) GBuf::kNorm);
            ImGui::SameLine();
            ImGui::RadioButton("depth", (int *) &(config->g_buf), (int) GBuf::kDepth);
            ImGui::RadioButton("id", (int *) &(config->g_buf), (int) GBuf::kId);
            ImGui::SameLine();
            ImGui::RadioButton("ao", (int *) &(config->g_buf), (int) GBuf::kAo);
            break;
    }

    ImGui::Separator();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();

    // Rendering
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

    glfwPollEvents();

    UpdateTime();
}

void WindowsHandler::SwapBuffer() {
    glfwSwapBuffers(window_);
}

void WindowsHandler::Init() {
    InitGlfw();
    InitGlad();
    InitImGui();
}

void WindowsHandler::InitGlfw() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    window_ = glfwCreateWindow(width_, height_, "sketchfab", nullptr, nullptr);
    assert(("Failed to create GLFW window", window_));

    glfwMakeContextCurrent(window_);

    glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
    glfwSetCursorPosCallback(window_, CursorPosCallback);
    glfwSetScrollCallback(window_, ScrollCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void WindowsHandler::InitGlad() {
    // glad: load all OpenGL function pointers
    bool load_glad = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    assert(("Failed to initialize GLAD", load_glad));
}

void WindowsHandler::InitImGui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void WindowsHandler::UpdateTime() {
    float current_time = glfwGetTime();
    delta_time_ = current_time - last_time_;
    last_time_ = current_time;
}

void WindowsHandler::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void WindowsHandler::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }
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
