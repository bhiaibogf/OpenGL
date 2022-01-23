#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>

#include <iostream>

#include "model.h"
#include "camera.h"

#include "light/directional_light.h"
#include "light/point_light.h"
#include "light/spot_light.h"
#include "light/sky_box.h"
#include "light/ibl.h"
#include "utils/depth_shower.h"
#include "utils/map_shower.h"
#include "transform.h"
#include "g_buffer.h"
#include "ssao.h"

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

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

GLFWwindow *Init() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
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
    glEnable(GL_DEPTH_TEST);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cube map sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // build and compile shaders
    Shader pbr_shader("shader/shading.vs", "shader/pbr.fs");
    Shader depth_shader("shader/depth.vs", "shader/depth.fs");
    Shader cube_shader("shader/cube.vs", "shader/cube.fs");

    // load models
    string path = "asset/nb574/";
    Model my_model(FileSystem::getPath(path + "nb574.obj"));

    Quad floor;
    Transform transform_floor;
    transform_floor.Scale(glm::vec3(10.f));
    transform_floor.Rotate(90, glm::vec3(1.0, 0.0, 0.0));
    transform_floor.Translate(glm::vec3(0, 0, 0.19));
    transform_floor.Update();

    Quad mirror;
    Transform transform_mirror;
    transform_mirror.Scale(glm::vec3(2.f));
    transform_mirror.Rotate(45 + 180, glm::vec3(1.0, 0.0, 0.0));
    transform_mirror.Translate(glm::vec3(0, -2, 0));
    transform_mirror.Update();

    auto albedo_map = TextureFromFile("nb574.jpg", FileSystem::getPath(path), false);
    auto normal_map = TextureFromFile("normals.jpg", FileSystem::getPath(path), false);
    auto ao_map = TextureFromFile("occlusion.jpg", FileSystem::getPath(path), false);

    SSAO ssao(kScrWidth, kScrHeight);

    // draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // set lights
    // directional light
    DirectionalLight directional_light(glm::vec3(1.f), glm::vec3(-0.2f, -1.0f, -0.3f));
    // point lights
    PointLight point_lights[4] = {
            PointLight(glm::vec3(1000.f), glm::vec3(20.f, 15.f, 20.f)),
            PointLight(glm::vec3(1000.f), glm::vec3(-23.f, 20.f, -40.f)),
            PointLight(glm::vec3(1000.f), glm::vec3(40.f, 25.f, -12.f)),
            PointLight(glm::vec3(1000.f), glm::vec3(-5.0f, 30.0f, -30.f))
    };
    // spotlight
    SpotLight spot_light(glm::vec3(0.0f), camera.get_position(), camera.get_front(),
                         glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
    path = FileSystem::getPath("resources/textures/hdr/newport_loft.hdr");
    IBL ibl(path);
    SkyBox sky_box(CubeMapCreator().ConvertFromEquirectangularMap(path));
    SkyBox sky(CubeMapCreator().ConvertFromSkyBox(FileSystem::getPath("resources/textures/skybox/")));


    // g_buffer
    GBuffer g_buffer(kScrWidth, kScrHeight);
    Quad quad;

    // for debug
    DepthShower depth_shower;
    MapShower map_shower;

    Transform transform;
    transform.Rotate(270.f, glm::vec3(1.0, 0.0, 0.0));
    transform.Update();

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // input
        processInput(window);

        // transformations
        auto x_translate = glm::translate(glm::mat4(1.f), camera.get_right() * x_off * 0.01f);
        auto y_translate = glm::translate(glm::mat4(1.f), camera.get_world_up() * y_off * 0.01f);
        auto x_rotate = glm::rotate(glm::mat4(1.f), glm::radians(yaw), camera.get_world_up());
        auto y_rotate = glm::rotate(glm::mat4(1.f), glm::radians(pitch), camera.get_right());
        transform.Update(y_rotate * x_rotate * y_translate * x_translate);

        transform.set_view(camera.GetViewMatrix());
        transform.set_projection(camera.GetProjectionMatrix());

        // 1. depth
        depth_shader.use();
        depth_shader.setMat4("model", transform.get_model());

        directional_light.SetDepthShader(depth_shader);
        my_model.Draw(depth_shader);

        directional_light.SetShader(g_buffer.get_l_shader());
        directional_light.SetShader(pbr_shader);

        for (int i = 0; i < 4; i++) {
            point_lights[i].SetDepthShader(depth_shader);
            my_model.Draw(depth_shader);

            point_lights[i].SetShader(g_buffer.get_l_shader(), i);
            point_lights[i].SetShader(pbr_shader, i);
        }

        spot_light.SetShader(pbr_shader);

        // 2. get g buffer
        // reset viewport
        glViewport(0, 0, kScrWidth, kScrHeight);

        g_buffer.BindGBuffer();
        g_buffer.get_g_shader().setMat4("uModel", transform.get_model());
        g_buffer.get_g_shader().setMat4("uView", transform.get_view());
        g_buffer.get_g_shader().setMat4("uProjection", transform.get_projection());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo_map);
        g_buffer.get_g_shader().setInt("uAlbedoMap", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normal_map);
        g_buffer.get_g_shader().setInt("uNormalMap", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ao_map);
        g_buffer.get_g_shader().setInt("uAoMap", 2);

        g_buffer.get_g_shader().setInt("uId", 1);
        my_model.Draw(g_buffer.get_g_shader());

        g_buffer.get_g_shader().setMat4("uModel", transform_floor.get_model());
        g_buffer.get_g_shader().setInt("uId", 2);
        floor.Draw();

        g_buffer.get_g_shader().setMat4("uModel", transform_mirror.get_model());
        g_buffer.get_g_shader().setInt("uId", 3);
        mirror.Draw();

        g_buffer.BindLBuffer();

        g_buffer.get_l_shader().setMat4("uModel", transform.get_model());
        g_buffer.get_l_shader().setMat4("uView", transform.get_view());
        g_buffer.get_l_shader().setMat4("uProjection", transform.get_projection());

        my_model.Draw(g_buffer.get_l_shader());

        g_buffer.get_l_shader().setMat4("uModel", transform_floor.get_model());
        floor.Draw();

        g_buffer.get_l_shader().setMat4("uModel", transform_mirror.get_model());
        mirror.Draw();

        g_buffer.UnbindLBuffer();

        // 3. ssao
        ssao.get_shader().use();
        ssao.get_shader().setMat4("uView", transform.get_view());
        ssao.get_shader().setMat4("uProjection", transform.get_projection());
        g_buffer.SetGBuffer(ssao.get_shader());

        ssao.Generate();
        ssao.Blur();

        // 4. render
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.f, 0.f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pbr_shader.use();

        g_buffer.SetGBuffer(pbr_shader);

        ibl.SetIblMap(pbr_shader);

        glActiveTexture(GL_TEXTURE20);
        glBindTexture(GL_TEXTURE_2D, ssao.get_ssao_blur());
        pbr_shader.setInt("gSsao", 20);

        pbr_shader.setVec3("camera_pos", camera.get_position());
        pbr_shader.setMat4("uWorldToScreen", transform.get_projection() * transform.get_view());
        pbr_shader.setMat4("uView", transform.get_view());

        pbr_shader.setBool("uAmbient", true);
        pbr_shader.setBool("uLo", true);
        pbr_shader.setBool("uAo", true);
        pbr_shader.setBool("uAoMap", true);
        pbr_shader.setBool("uIbl", true);

        quad.Draw();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer.get_fbo());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, kScrWidth, kScrHeight, 0, 0, kScrWidth, kScrHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        cube_shader.use();
        cube_shader.setMat4("view", transform.get_view());
        cube_shader.setMat4("projection", transform.get_projection());
        for (auto &point_light: point_lights) {
            point_light.Draw(cube_shader);
        }
        sky_box.Draw(camera);
        // sky.Draw(camera);
        // SkyBox(ibl.get_sky_box()).Draw(camera);
        // SkyBox(ibl.get_irradiance_map()).Draw(camera);
        // SkyBox(ibl.get_prefilter_map()).Draw(camera);

        // 5. debug
        // depth_shower.Show(directional_light);
        // depth_shower.Show(point_lights[0]);
        // depth_shower.Show(g_buffer.get_g_depth());

        // map_shower.Show(ibl.get_lut_map());
        // map_shower.Show(Sampler::GenerateNoiseMap());
        // map_shower.Show(ssao.get_ssao(), 0);
        // map_shower.Show(ssao.get_ssao_blur(), 0);
        // map_shower.Show(g_buffer.get_g_position());
        // map_shower.Show(g_buffer.get_g_normal_id());
        // map_shower.Show(g_buffer.get_g_albedo());
        // map_shower.Show(g_buffer.get_g_normal_id(), 3);
        // map_shower.Show(g_buffer.get_g_ao_metallic_roughness(), 0);
        // map_shower.Show(g_buffer.get_g_ao_metallic_roughness(), 1);
        // map_shower.Show(g_buffer.get_g_ao_metallic_roughness(), 2);

        // map_shower.Show(g_buffer.get_g_pos_dir_light());
        // map_shower.Show(g_buffer.get_g_pos_point_light()[0]);
        // map_shower.Show(g_buffer.get_g_pos_point_light()[1]);
        // map_shower.Show(g_buffer.get_g_pos_point_light()[2]);
        // map_shower.Show(g_buffer.get_g_pos_point_light()[3]);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::kForward, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::kBackward, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::kLeft, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::kRight, delta_time);
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
