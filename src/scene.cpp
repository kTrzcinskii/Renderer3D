//
// Created by Kacper Trzciński on 18.01.2025.
//

#include <utility>
#include <spdlog/spdlog.h>

#include "scene.h"

namespace Renderer3D {
    Scene::Scene(std::unordered_map<std::string, Entity> entities, std::unique_ptr<PointLightsContainer> pointLightsContainer) : _entities(std::move(entities)), _pointLightsContainer(std::move(pointLightsContainer))
    {
    }

    void Scene::AddEntity(const std::string& name, const Entity& entity)
    {
        _entities.emplace(name, entity);
    }

    void Scene::AddEntityUpdateFunction(const std::string& name, const UpdateEntityFunctionType& function)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto it = _entities.find(name);
        if (it == _entities.end())
        {
            spdlog::error("Trying to set entity update function for entity that is not in the scene: {}", name);
            return;
        }
        _updateEntityFunctions.emplace(name, function);
    }

    void Scene::UpdatePointLightContainer(std::unique_ptr<PointLightsContainer> pointLightsContainer)
    {
        _pointLightsContainer = std::move(pointLightsContainer);
    }

    void Scene::UpdateNightSkybox(std::unique_ptr<Skybox> skybox)
    {
        _nightSkybox = std::move(skybox);
    }

    void Scene::UpdateDaySkybox(std::unique_ptr<Skybox> skybox)
    {
        _daySkybox = std::move(skybox);
    }

    void Scene::UpdateEntities(const float deltaTime)
    {
        for (auto& [name, func] : _updateEntityFunctions)
        {
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto it = _entities.find(name);
            if (it != _entities.end())
            {
                func(it->second, deltaTime);
            }
            else
            {
                spdlog::error("Trying to update entity for entity that is not in the scene: {}", name);
            }
        }
    }

    void Scene::RenderEntitiesToGeometryPass(const std::shared_ptr<Shader>& geometryPassShader, const glm::mat4& view, const glm::mat4& projection) const
    {
        geometryPassShader->SetUniform("view", view);
        geometryPassShader->SetUniform("projection", projection);
        _floor.Draw(geometryPassShader);
        for (const auto& [_, entity] : _entities)
        {
           entity.Draw(geometryPassShader);
        }
    }

    void Scene::SetLightingPassShaderData(const std::shared_ptr<Shader>& lightingPassShader) const
    {
        _pointLightsContainer->SetLightingPassPointLightsData(lightingPassShader);
    }

    void Scene::RenderPointLightsForwardRendering(const glm::mat4& view, const glm::mat4& projection) const
    {
        _pointLightsContainer->RenderPointLights(view, projection);
    }

    void Scene::RenderNightSkyboxForwardRendering(const glm::mat4& view, const glm::mat4& projection) const
    {
        if (_nightSkybox == nullptr)
        {
            spdlog::error("Night skybox rendering not initialized");
            return;
        }
        _nightSkybox->Draw(view, projection);
    }

    void Scene::RenderDaySkyboxForwardRendering(const glm::mat4& view, const glm::mat4& projection) const
    {
        if (_daySkybox == nullptr)
        {
            spdlog::error("Day skybox rendering not initialized");
            return;
        }
        _daySkybox->Draw(view, projection);
    }
} // Renderer3D