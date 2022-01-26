#ifndef MESH_H
#define MESH_H

#include "base_model.h"
#include "vertex.h"
#include "texture.h"

#include <vector>

class Mesh : public BaseModel {
public:
    Mesh(const std::vector<Vertex> &vertices,
         const std::vector<unsigned int> &indices,
         const std::vector<Texture> &textures);

    // render the mesh
    void Draw() const override;

    void SetTextureAndDraw(const Shader &shader) const;

private:
    // mesh Data
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<Texture> textures_;

    unsigned int ebo_;

    // initializes all the buffer objects/arrays
    void setupMesh();
};

#endif
