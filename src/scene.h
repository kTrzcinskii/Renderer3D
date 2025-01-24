//
// Created by Kacper Trzciński on 18.01.2025.
//

#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>

#include "shader.h"
#include "entity.h"
#include "floor.h"
#include "point_lights_container.h"
#include "skybox.h"

namespace Renderer3D {
    using UpdateEntityFunctionType = std::function<void(Entity&,float)>;

    class Scene {
    public:
        explicit Scene(std::unordered_map<std::string, Entity> entities = std::unordered_map<std::string, Entity>(), std::unique_ptr<PointLightsContainer> pointLightsContainer = std::make_unique<PointLightsContainer>());
        void AddEntity(const std::string& name, const Entity& entity);
        void AddEntityUpdateFunction(const std::string& name, const UpdateEntityFunctionType& function);
        void UpdatePointLightContainer(std::unique_ptr<PointLightsContainer> pointLightsContainer);
        const std::unique_ptr<PointLightsContainer>& GetPointLightContainer() const;
        void UpdateNightSkybox(std::unique_ptr<Skybox> skybox);
        void UpdateDaySkybox(std::unique_ptr<Skybox> skybox);
        void UpdateEntities(float deltaTime);
        void RenderEntitiesToGeometryPass(const std::shared_ptr<Shader>& geometryPassShader, const glm::mat4& view, const glm::mat4& projection) const;
        void SetLightingPassShaderData(const std::shared_ptr<Shader>& lightingPassShader) const;
        void RenderPointLightsForwardRendering(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, bool useFog, float fogStrength, float cameraFarZ) const;
        void RenderNightSkyboxForwardRendering(const glm::mat4& view, const glm::mat4& projection) const;
        void RenderDaySkyboxForwardRendering(const glm::mat4& view, const glm::mat4& projection) const;
    private:
        std::unordered_map<std::string, Entity> _entities;
        std::unordered_map<std::string, UpdateEntityFunctionType> _updateEntityFunctions;
        std::unique_ptr<PointLightsContainer> _pointLightsContainer;
        Floor _floor;
        std::unique_ptr<Skybox> _nightSkybox = nullptr;
        std::unique_ptr<Skybox> _daySkybox = nullptr;
    };

} // Renderer3D

#endif //SCENE_H
