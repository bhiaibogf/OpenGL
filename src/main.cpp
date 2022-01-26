#include "light/directional_light.h"
#include "light/point_light.h"
#include "light/spot_light.h"
#include "light/sky_box.h"
#include "light/ibl.h"

#include "shower/depth_shower.h"
#include "shower/map_shower.h"

#include "camera/camera.h"

#include "model/scene.h"
#include "utils/shader.h"

#include "renderer/g_buffer.h"
#include "renderer/ssao.h"

#include "windows/windows_handler.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// settings
const unsigned int kScrWidth = 800;
const unsigned int kScrHeight = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
[[maybe_unused]] WindowsHandler windows_handler(kScrWidth, kScrHeight, &camera);

int main() {
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
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);
    // string path = "asset/objects/free_1972_datsun_240k_gt/gltf/";
    // Scene scene(path + "scene.gltf");
    string path = "asset/objects/nb574/";
    Scene scene(path + "nb574.obj");
    scene.Rotate(270.f, {1.0, 0.0, 0.0});

    Quad floor;
    floor.Scale(glm::vec3(10.f));
    floor.Translate({0, 0, 1.9});
    floor.Rotate(90, {1.0, 0.0, 0.0});

    Quad mirror;
    mirror.Scale(glm::vec3(2.f));
    mirror.Translate({0, -4, 0});
    mirror.Rotate(45 + 180, {1.0, 0.0, 0.0});

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
    SpotLight spot_light(glm::vec3(0.0f), camera.position(), camera.front(),
                         glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
    path = "asset/textures/hdr/newport_loft.hdr";
    IBL ibl(path);
    SkyBox sky_box(CubeMapCreator().ConvertFromEquirectangularMap(path));
    SkyBox sky(CubeMapCreator().ConvertFromSkyBox("asset/textures/skybox/"));

    // g_buffer
    GBuffer g_buffer(kScrWidth, kScrHeight);
    Quad quad;

    // for debug
    DepthShower depth_shower;
    MapShower map_shower;

    // render loop
    while (!WindowsHandler::ShouldClose()) {
        // per-frame time logic
        WindowsHandler::UpdateTime();

        // input
        WindowsHandler::ProcessInput();

        // transformations
        // x_translate
        scene.Translate(camera.right() * WindowsHandler::offset_x() * 0.01f);
        // y_translate
        scene.Translate(camera.world_up() * WindowsHandler::offset_y() * 0.01f);
        // x_rotate
        scene.Rotate(WindowsHandler::yaw(), camera.world_up());
        // y_rotate
        scene.Rotate(WindowsHandler::pitch(), camera.right());

        WindowsHandler::Clear();

        // 1. depth
        depth_shader.use();

        directional_light.SetDepthShader(depth_shader);
        scene.SetModelMatrixAndDraw(depth_shader);

        directional_light.SetShader(g_buffer.get_l_shader());
        directional_light.SetShader(pbr_shader);

        for (int i = 0; i < 4; i++) {
            point_lights[i].SetDepthShader(depth_shader);
            scene.Draw();

            point_lights[i].SetShader(g_buffer.get_l_shader(), i);
            point_lights[i].SetShader(pbr_shader, i);
        }

        spot_light.SetShader(pbr_shader);

        // 2. get g buffer
        // reset viewport
        glViewport(0, 0, kScrWidth, kScrHeight);

        g_buffer.BindGBuffer();

        camera.SetShader(g_buffer.get_g_shader());

        g_buffer.get_g_shader().setInt("uId", 1);
        scene.SetTextureAndDraw(g_buffer.get_g_shader());

        g_buffer.get_g_shader().setInt("uId", 2);
        floor.SetModelMatrixAndDraw(g_buffer.get_g_shader());

        g_buffer.get_g_shader().setInt("uId", 3);
        mirror.SetModelMatrixAndDraw(g_buffer.get_g_shader());

        g_buffer.BindLBuffer();

        camera.SetShader(g_buffer.get_l_shader());
        scene.SetModelMatrixAndDraw(g_buffer.get_l_shader());
        floor.SetModelMatrixAndDraw(g_buffer.get_l_shader());
        mirror.SetModelMatrixAndDraw(g_buffer.get_l_shader());

        g_buffer.UnbindLBuffer();

        // 3. ssao
        ssao.get_shader().use();
        camera.SetShader(ssao.get_shader());
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

        pbr_shader.setVec3("camera_pos", camera.position());
        camera.SetShader(pbr_shader);

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
        camera.SetShader(cube_shader);
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

        WindowsHandler::SwapBuffer();
    }
    return 0;
}
