//
// Created by bhiaibogf on 2022/1/25.
//

#ifndef SKETCHFAB_VERTEX_H
#define SKETCHFAB_VERTEX_H


#include <assimp/Importer.hpp>

struct Vertex {
    glm::vec3 position, normal, tangent, bitangent;
    glm::vec2 tex_coords;

    static void Set(const aiVector3t<float> &vv, glm::vec3 *v) {
        *v = glm::vec3(vv.x, vv.y, vv.z);
    }

    static void Set(const aiVector3t<float> &vv, glm::vec2 *v) {
        *v = glm::vec2(vv.x, vv.y);
    }
};

#endif //SKETCHFAB_VERTEX_H
