//
// Created by Kacper Trzciński on 16.01.2025.
//

#ifndef DEFERRED_SHADERER_H
#define DEFERRED_SHADERER_H

#include <glad/glad.h>

#include "shader.h"

namespace Renderer3D {

    class DeferredShaderer {
    public:
        DeferredShaderer(size_t width, size_t height);
        DeferredShaderer(DeferredShaderer&& shaderer) noexcept;
        void Resize(size_t width, size_t height);
        void BindGBuffer() const;
        void UnbindGBuffer() const;
        void BindGTextures() const;
        void CopyDepthBufferToDefaultBuffer() const;
        void RenderQuad() const;
        [[nodiscard]] std::shared_ptr<Shader> GetGeometryPassShader() const;
        [[nodiscard]] std::shared_ptr<Shader> GetLightingPassShader() const;
        ~DeferredShaderer();
    private:
        GLuint _gBuffer = 0;
        GLuint _gPosition = 0;
        GLuint _gNormal = 0;
        GLuint _gAlbedoSpec = 0;
        GLuint _rboDepth = 0;
        size_t _width;
        size_t _height;
        GLuint _quadVaoID = 0;
        GLuint _quadVboID = 0;
        bool _isMoved = false;
        std::shared_ptr<Shader> _geometryPassShader = nullptr;
        std::shared_ptr<Shader> _lightingPassShader = nullptr;
        // Helpers
        void CreatePositionBuffer(size_t width, size_t height);
        void CreateNormalBuffer(size_t width, size_t height);
        void CreateAlbedoSpecBuffer(size_t width, size_t height);
        void CreateDepthBuffer(size_t width, size_t height);
        void CheckFramebufferStatus() const;
        void SetupQuadData();
        void SetupLightingPassShader() const;
    };

} // Renderer3D

#endif //DEFERRED_SHADERER_H
