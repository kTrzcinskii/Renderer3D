//
// Created by Kacper Trzciński on 16.01.2025.
//

#include <spdlog/spdlog.h>

#include "deferred_shaderer.h"

namespace Renderer3D {
    DeferredShaderer::DeferredShaderer(const size_t width, const size_t height)
    {
        _width = width;
        _height = height;
        _geometryPassShader = std::make_shared<Shader>("../assets/shaders/model_geometry_pass_vertex.glsl", "../assets/shaders/model_geometry_pass_fragment.glsl");
        _lightingPassShader = std::make_shared<Shader>("../assets/shaders/model_lighting_pass_vertex.glsl", "../assets/shaders/model_lighting_pass_fragment.glsl");

        SetupQuadData();

        // Generate g buffer
        glGenFramebuffers(1, &_gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);

        CreatePositionBuffer(width, height);
        CreateNormalBuffer(width, height);
        CreateAlbedoSpecBuffer(width, height);

        // Set which color attachments are used for rendering
        // In our case we need 3, because we have multiple render targets
        constexpr unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        CreateDepthBuffer(width, height);

        // Check if there was any error
        CheckFramebufferStatus();

        // Cleanup state
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Setup gBuffer uniforms in lighting pass shader
        SetupLightingPassShader();
    }

    DeferredShaderer::DeferredShaderer(DeferredShaderer&& shaderer) noexcept
    {
        shaderer._isMoved = true;
        _gBuffer = shaderer._gBuffer;
        _gPosition = shaderer._gPosition;
        _gNormal = shaderer._gNormal;
        _gAlbedoSpec = shaderer._gAlbedoSpec;
        _rboDepth = shaderer._rboDepth;
        _width = shaderer._width;
        _height = shaderer._height;
        _quadVaoID = shaderer._quadVaoID;
        _quadVboID = shaderer._quadVboID;
        _geometryPassShader = shaderer._geometryPassShader;
        _lightingPassShader = shaderer._lightingPassShader;
    }

    void DeferredShaderer::Resize(const size_t width, const size_t height)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
        _width = width;
        _height = height;
        const auto previousGPosition = _gPosition;
        CreatePositionBuffer(width, height);
        glDeleteTextures(1, &previousGPosition);
        const auto previousGNormal = _gNormal;
        CreateNormalBuffer(width, height);
        glDeleteTextures(1, &previousGNormal);
        const auto previousGAlbedoSpec = _gAlbedoSpec;
        CreateAlbedoSpecBuffer(width, height);
        glDeleteTextures(1, &previousGAlbedoSpec);
        const auto previousDepthBuffer = _rboDepth;
        CreateDepthBuffer(width, height);
        glDeleteRenderbuffers(1, &previousDepthBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DeferredShaderer::BindGBuffer() const
    {

        glBindFramebuffer(GL_FRAMEBUFFER, _gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void DeferredShaderer::UnbindGBuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void DeferredShaderer::BindGTextures() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _gAlbedoSpec);
    }

    void DeferredShaderer::CopyDepthBufferToDefaultBuffer() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, _gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DeferredShaderer::RenderQuad() const
    {
        glBindVertexArray(_quadVaoID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    std::shared_ptr<Shader> DeferredShaderer::GetGeometryPassShader() const
    {
        return _geometryPassShader;
    }

    std::shared_ptr<Shader> DeferredShaderer::GetLightingPassShader() const
    {
        return _lightingPassShader;
    }

    DeferredShaderer::~DeferredShaderer()
    {
        if (_isMoved)
        {
            return;
        }
        if (_gBuffer != 0)
        {
            glDeleteFramebuffers(1, &_gBuffer);
        }
        if (_gPosition != 0)
        {
            glDeleteTextures(1, &_gPosition);
        }
        if (_gNormal != 0)
        {
            glDeleteTextures(1, &_gNormal);
        }
        if (_gAlbedoSpec != 0)
        {
            glDeleteTextures(1, &_gAlbedoSpec);
        }
        if (_rboDepth != 0)
        {
            glDeleteRenderbuffers(1, &_rboDepth);
        }
        if (_quadVaoID != 0)
        {
            glDeleteVertexArrays(1, &_quadVaoID);
        }
        if (_quadVboID != 0)
        {
            glDeleteBuffers(1, &_quadVboID);
        }
    }

    void DeferredShaderer::CreatePositionBuffer(const size_t width, const size_t height)
    {
        glGenTextures(1, &_gPosition);
        glBindTexture(GL_TEXTURE_2D, _gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition, 0);
    }

    void DeferredShaderer::CreateNormalBuffer(const size_t width, const size_t height)
    {
        glGenTextures(1, &_gNormal);
        glBindTexture(GL_TEXTURE_2D, _gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);
    }

    void DeferredShaderer::CreateAlbedoSpecBuffer(const size_t width, const size_t height)
    {
        glGenTextures(1, &_gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, _gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gAlbedoSpec, 0);
    }

    void DeferredShaderer::CreateDepthBuffer(size_t width, size_t height)
    {
        glGenRenderbuffers(1, &_rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, _rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rboDepth);
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void DeferredShaderer::CheckFramebufferStatus() const // NOLINT(*-convert-member-functions-to-static)
    {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            spdlog::error("Failed to create framebuffer!");

    }

    void DeferredShaderer::SetupQuadData()
    {
        constexpr float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &_quadVaoID);
        glGenBuffers(1, &_quadVboID);
        glBindVertexArray(_quadVaoID);
        glBindBuffer(GL_ARRAY_BUFFER, _quadVboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    }

    void DeferredShaderer::SetupLightingPassShader() const
    {
        _lightingPassShader->Activate();
        _lightingPassShader->SetUniform("gPosition", 0);
        _lightingPassShader->SetUniform("gNormal", 1);
        _lightingPassShader->SetUniform("gAlbedoSpec", 2);
    }
} // Renderer3D