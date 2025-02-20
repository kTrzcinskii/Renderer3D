//
// Created by Kacper Trzciński on 13.01.2025.
//

#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"

namespace Renderer3D {

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    class Mesh {
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::unordered_map<TextureType, std::vector<std::shared_ptr<Texture>>> textures);
        Mesh(Mesh&& mesh) noexcept;
        ~Mesh();
        void Draw(const Shader& shader) const;
        void Draw(const std::shared_ptr<Shader>& shader) const;
        // TODO: add support for DrawInstanced (?)
    private:
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
        std::unordered_map<TextureType, std::vector<std::shared_ptr<Texture>>> _textures;
        GLuint _vaoID;
        GLuint _vboID;
        GLuint _eboID;
        bool _isMoved = false;
    };

} // Renderer3D

#endif //MESH_H
