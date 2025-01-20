//
// Created by Kacper Trzciński on 16.01.2025.
//

#ifndef DEFERRED_SHADERER_H
#define DEFERRED_SHADERER_H

#include <glad/glad.h>

#include "shader.h"
#include "controls.h"

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
        void UpdateSceneMode(SceneMode sceneMode) const;
        void UpdateFogStrength(float fogStrength, float cameraFar) const;
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

        // Consts
        static constexpr float AMBIENT_LEVEL_NIGHT = 0.1f;
        static constexpr float AMBIENT_LEVEL_FOG = 0.3f;
        static constexpr float AMBIENT_LEVEL_DAY = 0.6f;
    };

} // Renderer3D

#endif //DEFERRED_SHADERER_H
