//
// Created by Kacper Trzciński on 13.01.2025.
//

#include <format>

#include "mesh.h"

#include "spdlog/spdlog.h"

namespace Renderer3D {
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
        std::unordered_map<TextureType, std::vector<std::shared_ptr<Texture>>> textures)
    {
        _vertices = std::move(vertices);
        _indices = std::move(indices);
        _textures = std::move(textures);

        // Generate buffers
        _vaoID = 0;
        glGenVertexArrays(1, &_vaoID);
        _vboID = 0;
        glGenBuffers(1, &_vboID);
        _eboID = 0;
        glGenBuffers(1, &_eboID);

        glBindVertexArray(_vaoID);

        // VBO data
        glBindBuffer(GL_ARRAY_BUFFER, _vboID);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(_vertices.size() * sizeof(Vertex)), _vertices.data(), GL_STATIC_DRAW);

        // EBO data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(_indices.size() * sizeof(unsigned int)), _indices.data(), GL_STATIC_DRAW);

        // Vertex position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Position)));
        glEnableVertexAttribArray(0);
        // Vertex normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));
        glEnableVertexAttribArray(1);
        // Vertex texture coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    Mesh::Mesh(Mesh&& mesh) noexcept
    {
        _vaoID = mesh._vaoID;
        _vboID = mesh._vboID;
        _eboID = mesh._eboID;
        _vertices = std::move(mesh._vertices);
        _indices = std::move(mesh._indices);
        _textures = std::move(mesh._textures);
        mesh._isMoved = true;
    }

    Mesh::~Mesh()
    {
        if (_isMoved)
        {
            return;
        }
        if (_vaoID != 0)
        {
            glDeleteVertexArrays(1, &_vaoID);
        }
        if (_vboID != 0)
        {
            glDeleteBuffers(1, &_vboID);
        }
        if (_eboID != 0)
        {
            glDeleteBuffers(1, &_eboID);
        }
    }

    void Mesh::Draw(const Shader& shader) const
    {
        // Setup textures
        shader.Activate();
        size_t textureUnit = 0;
        for (const auto& [type, textures]: _textures)
        {
            auto typeName = textureTypeToString(type);
            for (size_t i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + textureUnit);
                // We must use this struct name for our uniform inside shaders
                auto name = std::format("material.{}{}", typeName, i);
                shader.SetUniform(name, static_cast<int>(textureUnit));
                glBindTexture(GL_TEXTURE_2D, textures[i].get()->GetId());
                textureUnit++;
            }
        }

        // Draw mesh
        glBindVertexArray(_vaoID);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, nullptr);

        // Reset state
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(0);
    }

    void Mesh::Draw(const std::shared_ptr<Shader>& shader) const
    {
        // Setup textures
        shader->Activate();
        size_t textureUnit = 0;
        for (const auto& [type, textures]: _textures)
        {
            auto typeName = textureTypeToString(type);
            for (size_t i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + textureUnit);
                // We must use this struct name for our uniform inside shaders
                auto name = std::format("material.{}{}", typeName, i);
                shader->SetUniform(name, static_cast<int>(textureUnit));
                glBindTexture(GL_TEXTURE_2D, textures[i].get()->GetId());
                textureUnit++;
            }
        }

        // Draw mesh
        glBindVertexArray(_vaoID);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, nullptr);

        // Reset state
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(0);
    }
} // Renderer3D