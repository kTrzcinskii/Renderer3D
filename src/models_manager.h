//
// Created by Kacper Trzciński on 21.01.2025.
//

#ifndef MODELS_MANAGER_H
#define MODELS_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "model.h"

namespace Renderer3D {

    class ModelsManager {
    public:
        void AddModel(const std::string& name, const std::shared_ptr<Model>& model);
        std::shared_ptr<Model> GetModel(const std::string& name);
    private:
        std::unordered_map<std::string, std::shared_ptr<Model>> _models =  std::unordered_map<std::string, std::shared_ptr<Model>>();
    };

} // Renderer3D

#endif //MODELS_MANAGER_H
