//
// Created by Kacper Trzciński on 13.01.2025.
//

#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace Renderer3D {

    class Shader {
    public:
        Shader(const fs::path& vertexPath, const fs::path& fragmentPath);
        Shader(Shader&& other) noexcept;
        ~Shader();
        [[nodiscard]] GLuint GetProgramId() const;
        void Activate() const;
        void SetUniform(const std::string &name, bool value) const;
        void SetUniform(const std::string &name, int value) const;
        void SetUniform(const std::string &name, float value) const;
        void SetUniform(const std::string &name, const glm::vec2 &value) const;
        void SetUniform(const std::string &name, const glm::vec3 &value) const;
        void SetUniform(const std::string &name, const glm::vec4 &value) const;
        void SetUniform(const std::string &name, const glm::mat2 &value) const;
        void SetUniform(const std::string &name, const glm::mat3 &value) const;
        void SetUniform(const std::string &name, const glm::mat4 &value) const;
    private:
        GLuint _programID;
        bool _isMoved = false;
        // Helpers
        static std::string LoadShaderSource(const fs::path& path);
        static void CheckShaderCompilationResult(GLuint shaderId, const fs::path& path);
        static void CheckProgramLinkingResult(GLuint programId, const fs::path& vertexPath, const fs::path& fragmentPath);
        static constexpr size_t LOG_BUFFER_SIZE = 1024;
    };

} // Renderer3D

#endif //SHADER_H
