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

    std::string_view TextureTypeToString(TextureType type);
    TextureType textureTypeFromAssimp(aiTextureType type);

    class Texture {
    public:
        Texture(const fs::path& texturePath, TextureType type);
        ~Texture();
        [[nodiscard]] GLuint GetId() const;
        [[nodiscard]] TextureType GetType() const;
        [[nodiscard]] const fs::path& GetPath() const;
    private:
        GLuint _textureID;
        TextureType _type;
        fs::path _texturePath;
    };

} // Renderer3D

#endif //TEXTURE_H
