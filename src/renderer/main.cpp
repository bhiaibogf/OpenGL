#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/model.h>

#include <iostream>

#include "camera.h"
#include "light/directional_light.h"
#include "light/point_light.h"
#include "light/spot_light.h"
#include "debuger/depth_shower.h"
#include "g_buffer.h"
#include "debuger/map_shower.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void mouse_click_callback(GLFWwindow *window, int button, int action, int mods);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

float yaw = 0.f, pitch = 0.f;
float x_off = 0.f, y_off = 0.f;

// settings
const unsigned int kScrWidth = 800;
const unsigned int kScrHeight = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = kScrWidth / 2.0f;
float lastY = kScrHeight / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow *Init() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(kScrWidth, kScrHeight, "sketchfab", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_click_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);

    return window;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

int main() {
    GLFWwindow *window = Init();

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader pbr_shader("shader/shading.vs", "shader/pbr.fs");
    Shader depth_shader("shader/depth.vs", "shader/depth.fs");
    Shader cube_shader("shader/cube.vs", "shader/cube.fs");

    // load models
    // -----------
    string path = "asset/nb574/nb574.obj";
    Model my_model(FileSystem::getPath(path));

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // directional light
    DirectionalLight directional_light(glm::vec3(1.f), glm::vec3(-0.2f, -1.0f, -0.3f));

    // point light 1
    PointLight point_light[4] = {
            PointLight(glm::vec3(1000.f), glm::vec3(20.f, 15.f, 20.f)),
            PointLight(glm::vec3(1000.f), glm::vec3(-23.f, 20.f, -40.f)),
            PointLight(glm::vec3(1000.f), glm::vec3(40.f, 25.f, -12.f)),
            PointLight(glm::vec3(1000.f), glm::vec3(-5.0f, 30.0f, -30.f))
    };

    // spotLight
    SpotLight spot_light(glm::vec3(0.0f), camera.Position, camera.Front,
                         glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));

    DepthShower depth_shower;
    MapShower map_shower;

    GBuffer g_buffer(kScrWidth, kScrHeight);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // model transform
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) kScrWidth / (float) kScrHeight,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(glm::mat4(1.f), glm::radians(270.f), glm::vec3(1.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

        auto scale = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));

        auto x_translate = glm::translate(glm::mat4(1.f), camera.getRight() * x_off * 0.01f);
        auto y_translate = glm::translate(glm::mat4(1.f), camera.getWorldUp() * y_off * 0.01f);

        auto x_rotate = glm::rotate(glm::mat4(1.f), glm::radians(yaw), camera.getWorldUp());
        auto y_rotate = glm::rotate(glm::mat4(1.f), glm::radians(pitch), camera.getRight());

        model = y_rotate * x_rotate * y_translate * x_translate * model;

        depth_shader.use();
        depth_shader.setMat4("model", model);

        directional_light.SetDepthShader(depth_shader);
        my_model.Draw(depth_shader);

        directional_light.SetShader(g_buffer.get_shader());
        directional_light.SetShader(pbr_shader);

        for (int i = 0; i < 4; i++) {
            point_light[i].SetDepthShader(depth_shader);
            my_model.Draw(depth_shader);

            point_light[i].SetShader(g_buffer.get_shader(), i);
            point_light[i].SetShader(pbr_shader, i);
        }

        directional_light.SetShader(pbr_shader);
        spot_light.SetShader(pbr_shader);

        // reset viewport
        // render
        // ------
        glViewport(0, 0, kScrWidth, kScrHeight);

        g_buffer.Bind();
        g_buffer.get_shader().setMat4("model", model);
        g_buffer.get_shader().setMat4("view", view);
        g_buffer.get_shader().setMat4("projection", projection);
        my_model.Draw(g_buffer.get_shader());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pbr_shader.use();
        g_buffer.SetGBuffer(pbr_shader);
        pbr_shader.setVec3("camera_pos", camera.Position);
        renderQuad();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer.get_fbo());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, kScrWidth, kScrHeight, 0, 0, kScrWidth, kScrHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        cube_shader.use();
        cube_shader.setMat4("view", view);
        cube_shader.setMat4("projection", projection);
        for (int i = 0; i < 4; i++) {
            point_light[i].Draw(cube_shader);
        }

        // depth_shower.Show(directional_light);
        // depth_shower.Show(point_light[0]);

        // map_shower.Show(g_buffer.get_g_pos_dir_light());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

bool left_button_down = false, right_button_down = false, middle_button_down = false;

void mouse_click_callback(GLFWwindow *window, int button, int action, int mods) {
    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (GLFW_PRESS == action) {
                left_button_down = true;
                lastX = xpos;
                lastY = ypos;
            } else if (GLFW_RELEASE == action) {
                left_button_down = false;
            }
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (GLFW_PRESS == action) {
                right_button_down = true;
                lastX = xpos;
                lastY = ypos;
            } else if (GLFW_RELEASE == action) {
                right_button_down = false;
            }
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            if (GLFW_PRESS == action) {
                middle_button_down = true;
                lastX = xpos;
                lastY = ypos;
            } else if (GLFW_RELEASE == action) {
                middle_button_down = false;
            }
            break;
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    float x_offset = xpos - lastX;
    float y_offset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (left_button_down) {
        yaw += x_offset;
        pitch -= y_offset;
    }
    if (right_button_down) {
        x_off += x_offset;
        y_off += y_offset;
    }
    if (middle_button_down) {
        camera.ProcessMouseMovement(x_offset, y_offset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
