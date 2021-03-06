//
// Created by bhiaibogf on 2021/11/13.
//

#ifndef SKETCHFAB_IBL_H
#define SKETCHFAB_IBL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb/stb_image.h>

#include "../utils/shader.h"

#include "../model/cube.h"
#include "../model/quad.h"
#include "../utils/cube_map_creator.h"

class IBL {
public:
    IBL(const std::string &path);

    ~IBL();

    unsigned int get_sky_box() const { return sky_box_map_; }

    unsigned int get_irradiance_map() const { return irradiance_map_; }

    unsigned int get_prefilter_map() const { return prefilter_map_; }

    unsigned int get_lut_map() const { return brdf_lut_map_; }

    void SetIblMap(Shader &shader);

private:
    unsigned int fbo_;
    unsigned int rbo_;

    unsigned int sky_box_map_;

    const static int kIrradianceMapSize = 32;
    unsigned int irradiance_map_;
    Shader irradiance_shader_ = Shader("shader/cube_map.vs", "shader/irradiance_convolution.fs");
    // pbr: set up projection and view matrices for capturing data onto the 6 cube_map face directions
    glm::mat4 kCaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 kCaptureViews[6] = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    const static int kPrefilterMapSize = 128;
    unsigned int prefilter_map_;
    Shader prefilter_shader_ = Shader("shader/cube_map.vs", "shader/prefilter.fs");

    const static int kLutMapSize = 512;
    unsigned int brdf_lut_map_;
    Shader brdf_shader_ = Shader("shader/quad.vs", "shader/brdf.fs");

    Cube cube_;
    Quad quad_;

    void Prt();

    void BindFrameBuffer(unsigned int &map, int size);

    void GetIrradianceMap();

    void GetPrefilterMap();

    void GetLut();

};


#endif //SKETCHFAB_IBL_H
