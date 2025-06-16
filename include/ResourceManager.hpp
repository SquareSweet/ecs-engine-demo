#pragma once

#include "core/RenderComponent.hpp"
#include <memory>
#include <string>
#include <unordered_map>

// Using std::shared_ptr<Model>, so if several components refer the same model,
// the resource wouldn't unload too soon.
class ResourceManager {
public:
  ResourceManager() = default;

  // Load .obj (path) or return ptr if already loaded
  std::shared_ptr<Model> loadModel(const std::string &path);

private:
  std::unordered_map<std::string, std::shared_ptr<Model>> models;

  bool parseOBJ(const std::string &path, Model &outModel);
};