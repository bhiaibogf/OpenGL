//
// Created by bhiaibogf on 2021/11/13.
//

#ifndef SKETCHFAB_IBL_H
#define SKETCHFAB_IBL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <learnopengl/shader.h>

#include "../model/cube.h"
#include "../model/quad.h"
#include "../utils/cube_map_creator.h"

class IBL {
public:
    IBL(const std::string &path);

    ~IBL();

    void Draw(glm::mat4 view, glm::mat4 projection);

private:
    unsigned int captureFBO;
    unsigned int captureRBO;
    unsigned int envCubemap;
    unsigned int irradianceMap;
    unsigned int prefilterMap;
    Shader backgroundShader = Shader("2.2.2.background.vs", "2.2.2.background.fs");
    Shader irradianceShader = Shader("2.2.2.cubemap.vs", "2.2.2.irradiance_convolution.fs");
    Shader prefilterShader = Shader("2.2.2.cubemap.vs", "2.2.2.prefilter.fs");
    Shader brdfShader = Shader("2.2.2.brdf.vs", "2.2.2.brdf.fs");
    Cube cube_;
    Quad quad_;
    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[6] = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    void GetIrradianceMap();

};


#endif //SKETCHFAB_IBL_H
