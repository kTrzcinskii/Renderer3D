//
// Created by Kacper Trzciński on 13.01.2025.
//

#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

namespace Renderer3D {
    Shader::Shader(const fs::path& vertexPath, const fs::path& fragmentPath)
    {
        // Vertex shader
        const auto vertexShaderSource = LoadShaderSource(vertexPath);
        const auto vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderID, 1, reinterpret_cast<const GLchar* const*>(vertexShaderSource.c_str()), nullptr);
        glCompileShader(vertexShaderID);
        CheckShaderCompilationResult(vertexShaderID);

        // Fragment shader
        const auto fragmentShaderSource = LoadShaderSource(fragmentPath);
        const auto fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderID, 1, reinterpret_cast<const GLchar* const*>(fragmentShaderSource.c_str()), nullptr);
        glCompileShader(fragmentShaderID);
        CheckShaderCompilationResult(fragmentShaderID);

        // Create shader program
        _programID = glCreateProgram();
        glAttachShader(_programID, vertexShaderID);
        glAttachShader(_programID, fragmentShaderID);
        glLinkProgram(_programID);
        CheckProgramLinkingResult(_programID);

        // Cleanup
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }

    Shader::~Shader()
    {
        if (_programID != 0)
        {
            glDeleteProgram(_programID);
        }
    }

    GLuint Shader::GetProgramId() const
    {
        return _programID;
    }

    void Shader::Activate() const
    {
        glUseProgram(_programID);
    }

     void Shader::SetUniform(const std::string& name, const bool value) const
    {
        const auto location = glGetUniformLocation(_programID, name.c_str());
        if (location == -1)
        {
            spdlog::error("Shader trying to set invalid uniform: {}", name);
        }
        glUniform1i(location, static_cast<int>(value));
    }

    void Shader::SetUniform(const std::string& name, const int value) const
    {
        const auto location = glGetUniformLocation(_programID, name.c_str());
        if (location == -1)
        {
            spdlog::error("Shader trying to set invalid uniform: {}", name);
        }
        glUniform1i(location, value);
    }

    void Shader::SetUniform(const std::string& name, const float value) const
    {
        const auto location = glGetUniformLocation(_programID, name.c_str());
        if (location == -1)
        {
            spdlog::error("Shader trying to set invalid uniform: {}", name);
        }
        glUniform1f(location, value);
    }

    void Shader::SetUniform(const std::string& name, const glm::vec2& value) const
    {
        const auto location = glGetUniformLocation(_programID, name.c_str());
        if (location == -1)
        {
            spdlog::error("Shader trying to set invalid uniform: {}", name);
        }
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(const std::string& name, const glm::vec3& value) const
    {
        const auto location = glGetUniformLocation(_programID, name.c_str());
        if (location == -1)
        {
            spdlog::error("Shader trying to set invalid uniform: {}", name);
        }
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(const std::string& name, const glm::vec4& value) const
    {
        const auto location = glGetUniformLocation(_programID, name.c_str());
        if (location == -1)
        {
            spdlog::error("Shader trying to set invalid uniform: {}", name);
        }
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void Shader::SetUniform(const std::string& name, const glm::mat2& value) const
    {
        const auto location = glGetUniformLocation(_programID, name.c_str());
        if (location == -1)
        {
            spdlog::error("Shader trying to set invalid uniform: {}", name);
        }
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetUniform(const std::string& name, const glm::mat3& value) const
    {
        const auto location = glGetUniformLocation(_programID, name.c_str());
        if (location == -1)
        {
            spdlog::error("Shader trying to set invalid uniform: {}", name);
        }
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::SetUniform(const std::string& name, const glm::mat4& value) const
    {
        const auto location = glGetUniformLocation(_programID, name.c_str());
        if (location == -1)
        {
            spdlog::error("Shader trying to set invalid uniform: {}", name);
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    std::string Shader::LoadShaderSource(const fs::path& path)
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path.string());
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    void Shader::CheckShaderCompilationResult(const GLuint shaderId)
    {
        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[LOG_BUFFER_SIZE];
            glGetShaderInfoLog(shaderId, LOG_BUFFER_SIZE, nullptr, infoLog);
            spdlog::error("Shader compilation failed: {}", infoLog);
        }
    }

    void Shader::CheckProgramLinkingResult(const GLuint programId)
    {
        int success;
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[LOG_BUFFER_SIZE];
            glGetProgramInfoLog(programId, LOG_BUFFER_SIZE, nullptr, infoLog);
            spdlog::error("Shader program linking failed: {}", infoLog);
        }
    }
} // Renderer3D