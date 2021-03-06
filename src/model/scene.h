#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb/stb_image.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class Scene : public BaseModel {
public:
    // constructor, expects a filepath to a 3D model.
    explicit Scene(const std::string &path, bool need_gamma_correction = false);

    // draws the model, and thus all its meshes
    void Draw() const override;

    void SetTextureAndDraw(const Shader &shader) const;

private:
    std::vector<Texture> textures_;    // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes_;
    std::string directory_;
    bool need_gamma_correction_;

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void LoadModel(const std::string &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void ProcessNode(aiNode *node, const aiScene *scene);

    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> LoadTextures(aiMaterial *material, aiTextureType type, const std::string &type_name);

public:

};

#endif
