//
// Created by Kacper Trzciński on 13.01.2025.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <filesystem>
#include <assimp/material.h>

namespace fs = std::filesystem;

namespace Renderer3D {

    enum class TextureType
    {
        DIFFUSE,
        SPECULAR
    };

    std::string_view textureTypeToString(TextureType type);
    TextureType textureTypeFromAssimp(aiTextureType type);

    class Texture {
    public:
        Texture(const fs::path& texturePath, TextureType type);
        Texture(Texture&& texture) noexcept;
        ~Texture();
        [[nodiscard]] GLuint GetId() const;
        [[nodiscard]] TextureType GetType() const;
        [[nodiscard]] const fs::path& GetPath() const;
    private:
        GLuint _textureID;
        TextureType _type;
        fs::path _texturePath;
        bool _isMoved = false;
    };

} // Renderer3D

#endif //TEXTURE_H
