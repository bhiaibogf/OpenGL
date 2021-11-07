#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

#include <iostream>

#include "camera.h"
#include "light/directional_light.h"
#include "light/point_light.h"
#include "light/spot_light.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void mouse_click_callback(GLFWwindow *window, int button, int action, int mods);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

float yaw = 0.f, pitch = 0.f;
float x_off = 0.f, y_off = 0.f;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
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

GLFWwindow *Init() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "sketchfab", nullptr, nullptr);
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

int main() {
    GLFWwindow *window = Init();

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader pbr_shader("model.vs", "pbr.fs");
    Shader depth_shader("shadow_mapping_depth.vs", "shadow_mapping_depth.fs");
    Shader debug_shader("debug_quad.vs", "debug_quad_depth.fs");

    // load models
    // -----------
    string path = "asset/nb574/nb574.obj";
    Model my_model(FileSystem::getPath(path));

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
            glm::vec3(0.7f, 0.2f, 2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f, 2.0f, -12.0f),
            glm::vec3(0.0f, 0.0f, -3.0f)
    };

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // directional light
    DirectionalLight directional_light(glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(-0.2f, -1.0f, -0.3f));
    directional_light.SetDepthShader(depth_shader);
    directional_light.SetShader(pbr_shader);

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

        // point light 1
        PointLight point_light(glm::vec3(0.8f, 0.8f, 0.8f), pointLightPositions[0]);
        point_light.SetShader(pbr_shader, 0);

        // point light 2
        point_light = PointLight(glm::vec3(0.8f, 0.8f, 0.8f), pointLightPositions[1]);
        point_light.SetShader(pbr_shader, 1);

        // point light 3
        point_light = PointLight(glm::vec3(0.8f, 0.8f, 0.8f), pointLightPositions[2]);
        point_light.SetShader(pbr_shader, 2);

        // point light 4
        point_light = PointLight(glm::vec3(0.8f, 0.8f, 0.8f), pointLightPositions[3]);
        point_light.SetShader(pbr_shader, 3);

        // spotLight
        SpotLight spot_light(glm::vec3(0.8f, 0.8f, 0.8f) * 100.f, camera.Position, camera.Front,
                             glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
        spot_light.SetShader(pbr_shader);

        // model transform
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        pbr_shader.setMat4("projection", projection);
        pbr_shader.setMat4("view", view);

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

        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, directional_light.GetFBO());
        glClear(GL_DEPTH_BUFFER_BIT);
        depth_shader.setMat3("model", model);
        depth_shader.use();
        my_model.Draw(depth_shader);

        // reset viewport
        // render
        // ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pbr_shader.setMat4("model", model);
        // ourModel.Draw(pbr_shader);

        debug_shader.use();
        debug_shader.setFloat("near_plane", 1.0);
        debug_shader.setFloat("far_plane", 7.5);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, directional_light.GetDepthMap());
        renderQuad();

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
