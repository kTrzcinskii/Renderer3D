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

namespace Renderer3D {
    using UpdateEntityFunctionType = std::function<void(Entity&,float)>;

    class Scene {
    public:
        explicit Scene(std::unordered_map<std::string, Entity> entities = std::unordered_map<std::string, Entity>(), std::unique_ptr<PointLightsContainer> point_lights_container = std::make_unique<PointLightsContainer>());
        void AddEntity(const std::string& name, const Entity& entity);
        void AddEntityUpdateFunction(const std::string& name, const UpdateEntityFunctionType& function);
        void UpdatePointLightContainer(std::unique_ptr<PointLightsContainer> pointLightsContainer);
        void UpdateEntities(float deltaTime);
        void RenderEntitiesToGeometryPass(const std::shared_ptr<Shader>& geometryPassShader, const glm::mat4& view, const glm::mat4& projection) const;
        void SetLightingPassShaderData(const std::shared_ptr<Shader>& lightingPassShader) const;
        void RenderPointLightsForwardRendering(const glm::mat4& view, const glm::mat4& projection) const;
    private:
        std::unordered_map<std::string, Entity> _entities;
        std::unordered_map<std::string, UpdateEntityFunctionType> _updateEntityFunctions;
        std::unique_ptr<PointLightsContainer> _pointLightsContainer;
        Floor _floor;
    };

} // Renderer3D

#endif //SCENE_H
