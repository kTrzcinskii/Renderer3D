//
// Created by Kacper Trzciński on 22.01.2025.
//

#include <spdlog/spdlog.h>

#include "spot_lights_factory.h"

namespace Renderer3D {
    SpotLightsFactory::SpotLightsFactory()
    {
        for (size_t i = 0; i < SpotLightsFactory::MAX_NR_SPOT_LIGHTS; i++) // NOLINT(*-loop-convert)
        {
            _isUsed[i] = false;
        }
    }

    bool SpotLightsFactory::CanCreateSpotLight() const
    {
        return _nrSpotLights < MAX_NR_SPOT_LIGHTS;
    }

    SpotLightSource SpotLightsFactory::CreateSpotlight(const glm::vec3 position, const glm::vec3 direction, const float cutOff, const float outerCutOff)
    {
        if (!CanCreateSpotLight())
        {
            spdlog::error("Cannot create spotlight (limit was met)");
            throw std::runtime_error("Cannot create spotlight (limit was met)");
        }
        size_t index = 0;
        for (size_t i = 0; i < SpotLightsFactory::MAX_NR_SPOT_LIGHTS; i++)
        {
            if (!_isUsed[i])
            {
                _isUsed[i] = true;
                index = i;
                break;
            }
        }
        _nrSpotLights++;
        return SpotLightSource(index, position, direction, cutOff, outerCutOff); // NOLINT(*-return-braced-init-list)
    }

    void SpotLightsFactory::RemoveSpotlight(const size_t id, const std::shared_ptr<Shader>& lightingPassShader)
    {
        if (id >= MAX_NR_SPOT_LIGHTS || !_isUsed[id])
        {
            spdlog::error("Cannot remove spotlight (invalid id: {})", id);
            return;
        }
        _isUsed[id] = false;
        _nrSpotLights--;
        const auto name = std::format("spotLights[{}].use", id);
        lightingPassShader->SetUniform(name, false);
    }

    void SpotLightsFactory::SetSpotLightsCountUniform(const std::shared_ptr<Shader>& lightingPassShader) const
    {
        lightingPassShader->SetUniform("nrSpotLights", static_cast<int>(_nrSpotLights));
    }
} // Renderer3D