//
// Created by bhiaibogf on 2021/11/13.
//

#ifndef SKETCHFAB_CUBE_MAP_CREATOR_H
#define SKETCHFAB_CUBE_MAP_CREATOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <learnopengl/shader.h>

#include "../model/cube.h"

class CubeMapCreator {
public:
    CubeMapCreator();

    ~CubeMapCreator();

    unsigned int ConvertFromSkyBox();

    unsigned int ConvertFromEquirectangularMap(std::string path);

private:
    static const int kCaptureSize = 512;
    unsigned int fbo_;
    unsigned int depth_rbo_;
    unsigned int map_;

    Shader shader_ = Shader("shader/cube_map.vs", "shader/equirectangular_to_cube_map.fs");

    glm::mat4 kCaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 kCaptureViews[6] = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

};

#endif //SKETCHFAB_CUBE_MAP_CREATOR_H
