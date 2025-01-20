//
// Created by Kacper Trzciński on 17.01.2025.
//

#ifndef POINT_LIGHTS_CONTAINER_H
#define POINT_LIGHTS_CONTAINER_H

#include <vector>
#include <glad/glad.h>

#include "point_light_source.h"

namespace Renderer3D {

    class PointLightsContainer {
    public:
        explicit PointLightsContainer(const std::vector<PointLightSource>& pointLights = std::vector<PointLightSource>());
        PointLightsContainer(PointLightsContainer&& other) noexcept;
        ~PointLightsContainer();
        [[nodiscard]] bool CanAddPointLight() const;
        void AddPointLight(const PointLightSource& pointLight);
        void RenderPointLights(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, bool useFog, float fogStrength, float cameraFarZ) const;
        void SetLightingPassPointLightsData(const std::shared_ptr<Shader>& lightingPassShader) const;
    private:
        std::vector<PointLightSource> _pointLights;
        bool _isMoved = false;
        std::shared_ptr<Shader> _pointLightSourceShader = nullptr;
        // We store it here as every `PointLightSource` will use the same set of vertices
        GLuint _sphereVaoID = 0;
        GLuint _sphereVboID = 0;
        GLuint _sphereEboID = 0;
        std::vector<float> _sphereVertices;
        std::vector<unsigned int> _sphereIndices;
        // Helpers
        void GenerateVertices();
        void GenerateBuffers();
        void RenderSphere() const;
        // Consts
        // IMPORTANT: this value must match constant with the same name in lighting pass fragment shader
        static constexpr unsigned int MAX_NR_POINT_LIGHTS = 256;
        static constexpr int X_SEGMENTS = 64;
        static constexpr int Y_SEGMENTS = 64;
    };

} // Renderer3D

#endif //POINT_LIGHTS_CONTAINER_H
