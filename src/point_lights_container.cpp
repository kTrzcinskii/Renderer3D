//
// Created by Kacper Trzciński on 17.01.2025.
//

#include <glm/gtc/matrix_transform.hpp>

#include "point_lights_container.h"

namespace Renderer3D {

    PointLightsContainer::PointLightsContainer(const std::vector<PointLightSource>& pointLights)
    {
        _pointLights = pointLights;
        GenerateVertices();
        GenerateBuffers();
        _pointLightSourceShader = std::make_shared<Shader>("../assets/shaders/light_source_vertex.glsl", "../assets/shaders/light_source_fragment.glsl");
    }

    PointLightsContainer::PointLightsContainer(PointLightsContainer&& other) noexcept
    {
        other._isMoved = true;
        _pointLights = std::move(other._pointLights);
        _sphereVaoID = other._sphereVaoID;
        _sphereVboID = other._sphereVboID;
        _sphereEboID = other._sphereEboID;
        _sphereVertices = std::move(other._sphereVertices);
        _sphereIndices = std::move(other._sphereIndices);
        _pointLightSourceShader = std::move(other._pointLightSourceShader);
    }

    PointLightsContainer::~PointLightsContainer()
    {
        if (_isMoved)
        {
            return;
        }
        if (_sphereVaoID != 0)
        {
            glDeleteVertexArrays(1, &_sphereVaoID);
        }
        if (_sphereVboID != 0)
        {
            glDeleteBuffers(1, &_sphereVboID);
        }
        if (_sphereEboID != 0)
        {
            glDeleteBuffers(1, &_sphereEboID);
        }
    }

    bool PointLightsContainer::CanAddPointLight() const
    {
        return _pointLights.size() < MAX_NR_POINT_LIGHTS;
    }

    void PointLightsContainer::AddPointLight(const PointLightSource& pointLight)
    {
        if (CanAddPointLight())
        {
            _pointLights.push_back(pointLight);
        }
    }

    void PointLightsContainer::RenderPointLights(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, const bool useFog, const float fogStrength, const float cameraFarZ) const
    {
        // Render light sources using forward rendering
        _pointLightSourceShader->Activate();
        _pointLightSourceShader->SetUniform("view", view);
        _pointLightSourceShader->SetUniform("projection", projection);
        _pointLightSourceShader->SetUniform("cameraPos", cameraPos);
        _pointLightSourceShader->SetUniform("useFog", useFog);
        _pointLightSourceShader->SetUniform("fogMaxDist", cameraFarZ - fogStrength);
        for (const auto & _pointLight : _pointLights)
        {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, _pointLight.GetPosition());
            model = glm::scale(model, glm::vec3(0.125f));
            _pointLightSourceShader->SetUniform("model", model);
            _pointLightSourceShader->SetUniform("lightColor", _pointLight.GetColor());
            RenderSphere();
        }
    }

    void PointLightsContainer::SetLightingPassPointLightsData(const std::shared_ptr<Shader>& lightingPassShader) const
    {
        lightingPassShader->SetUniform("nrPointLights", static_cast<int>(_pointLights.size()));
        for (size_t i = 0; i < _pointLights.size(); i++)
        {
            _pointLights[i].SetUniforms(lightingPassShader, i);
        }
    }

    void PointLightsContainer::GenerateVertices()
    {
        // Generate vertices
        for (unsigned int y = 0; y <= PointLightsContainer::Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= PointLightsContainer::X_SEGMENTS; ++x)
            {
                const auto xSegment = static_cast<float>(x) / PointLightsContainer::X_SEGMENTS;
                const auto ySegment = static_cast<float>(y) / PointLightsContainer::Y_SEGMENTS;
                const auto xPos = static_cast<float>(std::cos(xSegment * 2.0f * std::numbers::pi) * std::sin(ySegment * std::numbers::pi));
                const auto yPos = static_cast<float>(std::cos(ySegment * std::numbers::pi));
                const auto zPos = static_cast<float>(std::sin(xSegment * 2.0f * std::numbers::pi) * std::sin(ySegment * std::numbers::pi));

                // Vertex position
                _sphereVertices.push_back(xPos);
                _sphereVertices.push_back(yPos);
                _sphereVertices.push_back(zPos);

                // Normal (same as position for unit sphere)
                _sphereVertices.push_back(xPos);
                _sphereVertices.push_back(yPos);
                _sphereVertices.push_back(zPos);
            }
        }

        // Generate indices
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x < X_SEGMENTS; ++x)
            {
                unsigned int first = (y * (X_SEGMENTS + 1)) + x;
                unsigned int second = first + X_SEGMENTS + 1;

                _sphereIndices.push_back(first);
                _sphereIndices.push_back(second);
                _sphereIndices.push_back(first + 1);

                _sphereIndices.push_back(second);
                _sphereIndices.push_back(second + 1);
                _sphereIndices.push_back(first + 1);
            }
        }
    }

    void PointLightsContainer::GenerateBuffers()
    {
        // IMPORTANT: at the point of calling this function both `_sphereVertices` and `_sphereIndicies`
        // must be set up

        // Generate buffers
        glGenVertexArrays(1, &_sphereVaoID);
        glGenBuffers(1, &_sphereVboID);
        glGenBuffers(1, &_sphereEboID);

        // Bind vao
        glBindVertexArray(_sphereVaoID);

        // Bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, _sphereVboID);
        glBufferData(GL_ARRAY_BUFFER, _sphereVertices.size() * sizeof(float), &_sphereVertices[0], GL_STATIC_DRAW);

        // Bind ebo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _sphereEboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _sphereIndices.size() * sizeof(unsigned int), &_sphereIndices[0], GL_STATIC_DRAW);

        // Setup attrib pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void PointLightsContainer::RenderSphere() const
    {
        glBindVertexArray(_sphereVaoID);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_sphereIndices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

} // Renderer3D