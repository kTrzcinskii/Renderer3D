//
// Created by Kacper Trzciński on 18.01.2025.
//


#include "floor.h"

namespace Renderer3D {
    Floor::Floor()
    {
        // Setup buffers
        glGenVertexArrays(1, &_vaoId);
        glGenBuffers(1, &_vboId);
        glGenBuffers(1, &_eboId);

        glBindVertexArray(_vaoId);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, _vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(FLOOR_VERTICES), FLOOR_VERTICES, GL_STATIC_DRAW);

        // Element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(FLOOR_INDICES), FLOOR_INDICES, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // Cleanup
        glBindVertexArray(0);
    }

    Floor::Floor(Floor&& other) noexcept
    {
        other._isMoved = true;
        _vaoId = other._vaoId;
        _vboId = other._vboId;
        _eboId = other._eboId;
        _model = other._model;
    }

    Floor::~Floor()
    {
        if (_isMoved)
        {
            return;
        }
        if (_vaoId != 0)
        {
            glDeleteVertexArrays(1, &_vaoId);
        }
        if (_vboId != 0)
        {
            glDeleteBuffers(1, &_vboId);
        }
        if (_eboId != 0)
        {
            glDeleteBuffers(1, &_eboId);
        }
    }

    void Floor::Draw(const std::shared_ptr<Shader>& shader) const
    {
        // Setup model matrix
        shader->SetUniform("model", _model);
        // Setup texture
        shader->SetUniform("material.diffuse0", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture.GetId());
        // Render
        glBindVertexArray(_vaoId);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // Cleanup
        glBindVertexArray(0);
    }
} // Renderer3D