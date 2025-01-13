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
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::unordered_map<TextureType, std::vector<Texture>> textures);
        ~Mesh();
        void Draw(const Shader& shader);
        // TODO: add support for DrawInstanced (?)
    private:
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
        std::unordered_map<TextureType, std::vector<Texture>> _textures;
        GLuint _vaoID;
        GLuint _vboID;
        GLuint _eboID;
    };

} // Renderer3D

#endif //MESH_H
