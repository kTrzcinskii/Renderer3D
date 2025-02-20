//
// Created by Kacper Trzciński on 14.01.2025.
//

#ifndef MODEL_H
#define MODEL_H

#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "mesh.h"
#include "shader.h"

namespace fs = std::filesystem;

namespace Renderer3D {

    class Model {
    public:
        explicit Model(const fs::path& path, bool flipTextures = false);
        void Draw(const Shader& shader) const;
        void Draw(const std::shared_ptr<Shader>& shader) const;
    private:
        std::vector<Mesh> _meshes;
        fs::path _directory;
        std::unordered_map<fs::path, std::shared_ptr<Texture>> _loadedTextures;
        void ProcessNode(const aiNode *node, const aiScene *scene);
        Mesh ProcessMesh(const aiMesh *mesh, const aiScene *scene);
        std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(const aiMaterial *material, aiTextureType assimpType);
    };

} // Renderer3D

#endif //MODEL_H
