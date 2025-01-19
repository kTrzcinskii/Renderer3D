//
// Created by Kacper Trzciński on 19.01.2025.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"

namespace fs = std::filesystem;

namespace Renderer3D {

    class Skybox {
    public:
        Skybox(const fs::path& right, const fs::path& left, const fs::path& top, const fs::path& bottom,
        const fs::path& front, const fs::path& back, const std::shared_ptr<Shader>& shader);
        Skybox(Skybox&& other) noexcept;
        ~Skybox();
        void Draw(const glm::mat4& view, const glm::mat4& projection) const;
    private:
        GLuint _cubemapID = 0;
        GLuint _vaoID = 0;
        GLuint _vboID = 0;
        GLuint _eboID = 0;
        std::shared_ptr<Shader> _shader = nullptr;
        bool _isMoved = false;

        // Helpers
        void LoadCubemap(const fs::path faces[6]);
        void GenerateBuffers();

        // Consts
        static constexpr float SKYBOX_VERTICES[] = {
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f
        };
        static constexpr unsigned int SKYBOX_INDICES[] = {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4,
            0, 3, 7,
            7, 4, 0,
            1, 5, 6,
            6, 2, 1,
            3, 2, 6,
            6, 7, 3,
            0, 4, 5,
            5, 1, 0
        };
    };

} // Renderer3D

#endif //SKYBOX_H
