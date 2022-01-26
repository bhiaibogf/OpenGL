//
// Created by bhiaibogf on 2022/1/26.
//

#include "texture.h"

Texture::Texture(const std::string &path, const std::string &type_name)
        : path_(path), type_name_(type_name) {
    id_ = TextureFromFile(path_);
}

bool Texture::Is(const std::string &path) const {
    return path_ == path;
}

void Texture::Set(const Shader &shader) const {
    unsigned int type = -1;
    if (type_name_ == "uAlbedoMap") {
        type = 0;
    } else if (type_name_ == "uNormalMap") {
        type = 1;
    } else if (type_name_ == "uAoMap") {
        type = 2;
    }
    glActiveTexture(GL_TEXTURE0 + type);
    glBindTexture(GL_TEXTURE_2D, id_);
    shader.setInt(type_name_, type);
}

unsigned int Texture::TextureFromFile(const std::string &path, bool need_gamma_correction) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int width, height, channels;
    auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data) {
        GLenum format;
        switch (channels) {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                return -1;
        }

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return texture_id;
}
