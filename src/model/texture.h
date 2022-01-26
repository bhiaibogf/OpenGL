//
// Created by bhiaibogf on 2022/1/25.
//

#ifndef SKETCHFAB_TEXTURE_H
#define SKETCHFAB_TEXTURE_H


#include "../utils/shader.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include <string>
#include <iostream>

class Texture {
public:
    Texture(const std::string &path, const std::string &type_name);

    ~Texture() = default;

    bool Is(const std::string &path) const;

    void Set(const Shader &shader) const;

private:
    unsigned int id_;
    std::string type_name_;
    std::string path_;

    static unsigned int
    TextureFromFile(const std::string &path, bool need_gamma_correction = false);

};

#endif //SKETCHFAB_TEXTURE_H
