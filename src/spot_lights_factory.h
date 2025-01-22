//
// Created by Kacper Trzciński on 22.01.2025.
//

#ifndef SPOT_LIGHTS_FACTORY_H
#define SPOT_LIGHTS_FACTORY_H

#include "spot_light_source.h"

namespace Renderer3D {

    class SpotLightsFactory {
    public:
        SpotLightsFactory();
        [[nodiscard]] bool CanCreateSpotLight() const;
        SpotLightSource CreateSpotlight(glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff);
        void RemoveSpotlight(size_t id, const std::shared_ptr<Shader>& lightingPassShader);
        void SetSpotLightsCountUniform(const std::shared_ptr<Shader>& lightingPassShader) const;
    private:
        // Consts
        // IMPORTANT: this value must exactly match the value with same name from lighting pass shader
        static constexpr size_t MAX_NR_SPOT_LIGHTS = 16;

        bool _isUsed[SpotLightsFactory::MAX_NR_SPOT_LIGHTS] = {};
        size_t _nrSpotLights = 0;
    };

} // Renderer3D

#endif //SPOT_LIGHTS_FACTORY_H
