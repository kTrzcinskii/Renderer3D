//
// Created by Kacper Trzciński on 14.01.2025.
//

#include <assimp/postprocess.h>
#include <spdlog/spdlog.h>
#include <stb_image/stb_image.h>

#include "model.h"

namespace Renderer3D {
    Model::Model(const fs::path& path, const bool flipTextures)
    {
        stbi_set_flip_vertically_on_load(flipTextures);
        Assimp::Importer importer;
        const auto scene = importer.ReadFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            spdlog::error("Failed to load model from {} ({})", path.string(), importer.GetErrorString());
            return;
        }
        _directory = path.parent_path();
        ProcessNode(scene->mRootNode, scene);
    }

    void Model::Draw(const Shader& shader) const
    {
        for (auto &mesh: _meshes)
        {
            mesh.Draw(shader);
        }
    }

    void Model::Draw(const std::shared_ptr<Shader>& shader) const
    {
        for (auto &mesh: _meshes)
        {
            mesh.Draw(shader);
        }
    }

    void Model::ProcessNode(const aiNode* node, const aiScene* scene) // NOLINT(*-no-recursion)
    {
        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            const auto mesh = scene->mMeshes[node->mMeshes[i]];
            auto processedMesh = ProcessMesh(mesh, scene);
            _meshes.emplace_back(std::move(processedMesh));
        }
        for (size_t i = 0; i < node->mNumChildren; i++)
        {
            const auto childNode = node->mChildren[i];
            ProcessNode(childNode, scene);
        }
    }

    Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::unordered_map<TextureType, std::vector<std::shared_ptr<Texture>>> textures;

        // Vertices
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex{};
            // Position
            glm::vec3 position;
            position.x = mesh->mVertices[i].x;
            position.y = mesh->mVertices[i].y;
            position.z = mesh->mVertices[i].z;
            vertex.Position = position;

            // Normal
            glm::vec3 normal;
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].y;
            normal.z = mesh->mNormals[i].z;
            vertex.Normal = normal;

            // Texture coords
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 texCoords;
                texCoords.x = mesh->mTextureCoords[0][i].x;
                texCoords.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = texCoords;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.emplace_back(vertex);
        }

        // Indices
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            const auto face = mesh->mFaces[i];
            for (size_t j = 0; j < face.mNumIndices; j++)
            {
                indices.emplace_back(face.mIndices[j]);
            }
        }

        // Textures
        auto material = scene->mMaterials[mesh->mMaterialIndex];
        auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
        textures.insert(std::pair(TextureType::DIFFUSE, diffuseMaps));
        auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
        textures.insert(std::pair(TextureType::SPECULAR, specularMaps));

        return {vertices, indices, textures};
    }

    std::vector<std::shared_ptr<Texture>> Model::LoadMaterialTextures(const aiMaterial* material, const aiTextureType assimpType)
    {
        std::vector<std::shared_ptr<Texture>> textures;
        for (size_t i = 0; i < material->GetTextureCount(assimpType); i++)
        {
            aiString str;
            material->GetTexture(assimpType, i, &str);
            fs::path path(_directory);
            path.append(str.C_Str());

            // ReSharper disable once CppTooWideScopeInitStatement
            const auto loadedId = _loadedTextures.find(path);
            if (loadedId != _loadedTextures.end())
            {
                textures.push_back(loadedId->second);
            }
            else
            {
                const auto type = textureTypeFromAssimp(assimpType);
                auto texture = std::make_shared<Texture>(Texture(path, type));
                textures.push_back(texture);
                _loadedTextures.insert(std::pair(path, texture));
            }
        }
        return textures;
    }
} // Renderer3D