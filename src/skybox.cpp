//
// Created by Kacper Trzciński on 19.01.2025.
//

#include <stb_image/stb_image.h>
#include <spdlog/spdlog.h>

#include "skybox.h"

namespace Renderer3D {
    Skybox::Skybox(const fs::path& right, const fs::path& left, const fs::path& top, const fs::path& bottom,
        const fs::path& front, const fs::path& back, const std::shared_ptr<Shader>& shader) : _shader(shader)
    {
        const fs::path faces[] = {right, left, top, bottom, front, back};
        LoadCubemap(faces);
        GenerateBuffers();
    }

    Skybox::Skybox(Skybox&& other) noexcept
    {
        other._isMoved = true;
        _cubemapID = other._cubemapID;
        _vaoID = other._vaoID;
        _vboID = other._vboID;
        _eboID = other._eboID;
        _shader = other._shader;
    }

    Skybox::~Skybox()
    {
        if (_isMoved)
        {
            return;
        }
        if (_cubemapID != 0)
        {
            glDeleteTextures(1, &_cubemapID);
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

    void Skybox::Draw(const glm::mat4& view, const glm::mat4& projection) const
    {
        // We change it so that test passes even when values are equal,
        // meaning skybox will be rendered only if there is actually no other object visible before it
        glDepthFunc(GL_LEQUAL);
        _shader->Activate();
        _shader->SetUniform("projection", projection);
        // Remove translation from the view matrix so that camera is always in the center of the skybox
        // We just need to remove last row and column
        const auto skyboxView = glm::mat4(glm::mat3(view));
        _shader->SetUniform("view", skyboxView);
        _shader->SetUniform("skybox", 0);
        // Render
        glBindVertexArray(_vaoID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemapID);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        // Cleanup
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

    void Skybox::LoadCubemap(const fs::path faces[6])
    {
        glGenTextures(1, &_cubemapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemapID);

        int width, height, nrComponents;

        for (size_t i = 0; i < 6; i++)
        {
            // ReSharper disable once CppTooWideScope
            const auto data = stbi_load(faces[i].string().c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format;
                switch (nrComponents)
                {
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
                    spdlog::error("Texture format not supported (invalid number of components: {})", nrComponents);
                    stbi_image_free(data);
                    glDeleteTextures(1, &_cubemapID);
                    return;
                }

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, static_cast<GLint>(format), width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                spdlog::error("Failed to load cubemap {}", faces[i].string());
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // Cleanup
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void Skybox::GenerateBuffers()
    {
        // Generate buffers
        glGenVertexArrays(1, &_vaoID);
        glGenBuffers(1, &_vboID);
        glGenBuffers(1, &_eboID);

        // Bind vao
        glBindVertexArray(_vaoID);

        // Bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, _vboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTICES), &SKYBOX_VERTICES[0], GL_STATIC_DRAW);

        // Bind ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _eboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SKYBOX_INDICES), &SKYBOX_INDICES[0], GL_STATIC_DRAW);

        // Setup attrib pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
} // Renderer3D