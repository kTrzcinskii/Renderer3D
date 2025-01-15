//
// Created by Kacper Trzciński on 13.01.2025.
//

#include <cstdint>
#include <stb_image/stb_image.h>
#include <spdlog/spdlog.h>

#include "texture.h"

namespace Renderer3D {
    std::string_view textureTypeToString(const TextureType type)
    {
        switch (type)
        {
        case TextureType::DIFFUSE:
            return "diffuse";
        case TextureType::SPECULAR:
            return "specular";
        default:
            throw std::invalid_argument("Invalid enum value");
        }
    }

    TextureType textureTypeFromAssimp(const aiTextureType type)
    {
        switch (type)
        {
            case aiTextureType_DIFFUSE:
                return TextureType::DIFFUSE;
            case aiTextureType_SPECULAR:
                return TextureType::SPECULAR;
            default:
                throw std::invalid_argument("Unsupported enum value");
        }
    }

    Texture::Texture(const fs::path& texturePath, const TextureType type)
    {
        _type = type;
        _texturePath = texturePath;
        _textureID = 0;
        glGenTextures(1, &_textureID);

        int width, height, nrComponents;

        uint8_t* data = stbi_load(texturePath.string().c_str(), &width, &height, &nrComponents, 0);

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
                glDeleteTextures(1, &_textureID);
                return;
            }

            glBindTexture(GL_TEXTURE_2D, _textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            spdlog::error("Failed to load texture at {}", texturePath.string());
        }

        stbi_image_free(data);
    }

    Texture::Texture(Texture&& texture) noexcept
    {
        texture._isMoved = true;
        _textureID = texture._textureID;
        _texturePath = texture._texturePath;
        _type = texture._type;
    }

    Texture::~Texture()
    {
        if (_isMoved)
        {
            return;
        }
        if (_textureID != 0)
        {
            glDeleteTextures(1, &_textureID);
        }
    }

    GLuint Texture::GetId() const
    {
        return _textureID;
    }

    TextureType Texture::GetType() const
    {
        return _type;
    }

    const fs::path& Texture::GetPath() const
    {
        return _texturePath;
    }
} // Renderer3D