//
// Created by Kacper Trzciński on 21.01.2025.
//

#include "models_manager.h"

namespace Renderer3D {
    void ModelsManager::AddModel(const std::string& name, const std::shared_ptr<Model>& model)
    {
        _models.insert(std::pair(name, model));
    }

    std::shared_ptr<Model> ModelsManager::GetModel(const std::string& name)
    {
        const auto it = _models.find(name);
        if (it == _models.end())
        {
            return nullptr;
        }
        return it->second;
    }
} // Renderer3D