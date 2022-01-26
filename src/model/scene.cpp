//
// Created by bhiaibogf on 2022/1/25.
//

#include "scene.h"

Scene::Scene(string const &path, bool need_gamma_correction) : need_gamma_correction_(need_gamma_correction) {
    LoadModel(path);
}

void Scene::Draw() const {
    for (const auto &mesh: meshes_)
        mesh.Draw();
}

void Scene::Draw(const Shader &shader) const {
    BaseModel::Draw(shader);
    Draw();
}

void Scene::LoadModel(const string &path) {
    // read file via ASSIMP
    Assimp::Importer importer;
    auto scene = importer.ReadFile(path,
                                   aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                   aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    // retrieve the directory path of the filepath
    directory_ = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene);
}

void Scene::ProcessNode(aiNode *node, const aiScene *scene) {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(ProcessMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }

}

Mesh Scene::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // positions
        Vertex::Set(mesh->mVertices[i], &vertex.position);

        // normals
        if (mesh->HasNormals()) {
            Vertex::Set(mesh->mNormals[i], &vertex.normal);
        }

        // texture coordinates
        if (mesh->HasTextureCoords(0)) {
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates, so we always take the first set (0).
            Vertex::Set(mesh->mTextureCoords[0][i], &vertex.tex_coords);
        } else {
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->HasTangentsAndBitangents()) {
            Vertex::Set(mesh->mTangents[i], &vertex.tangent);
            Vertex::Set(mesh->mBitangents[i], &vertex.bitangent);
        }
        vertices.push_back(vertex);
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process materials
    auto material = scene->mMaterials[mesh->mMaterialIndex];

    // for (enum aiTextureType type = aiTextureType_NONE;
    //      type <= AI_TEXTURE_TYPE_MAX;
    //      type = (aiTextureType) (type + 1)) {
    //     cout << type << ' ' << material->GetTextureCount(type) << endl;
    // }

    // diffuse maps
    vector<Texture> diffuse_maps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "uAlbedoMap");
    // 2. specular maps
    // vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    // normal maps
    std::vector<Texture> normal_maps = loadMaterialTextures(material, aiTextureType_NORMALS, "uNormalMap");
    // ao maps
    std::vector<Texture> ao_maps = loadMaterialTextures(material, aiTextureType_AMBIENT, "uAoMap");

    // return a mesh object created from the extracted mesh data
    return {vertices, indices, textures};
}

vector<Texture> Scene::loadMaterialTextures(aiMaterial *material, aiTextureType type, const string &type_name) {
    vector<Texture> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
        aiString path;
        material->GetTexture(type, i, &path);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (auto &texture: textures_) {
            if (texture.Is(path.C_Str())) {
                textures.push_back(texture);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) {   // if texture hasn't been loaded already, load it
            Texture texture(directory_ + '/' + path.C_Str(), type_name);
            textures.push_back(texture);
            // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            textures_.push_back(texture);
        }
    }
    return textures;
}

void Scene::SetTexture(const Shader &shader) const {
    for (auto &texture: textures_) {
        texture.Set(shader);
    }
}
