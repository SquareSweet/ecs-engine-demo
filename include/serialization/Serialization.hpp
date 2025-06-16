#pragma once

#include <string>
#include "core/World.hpp"
#include "ResourceManager.hpp"

// returns true if successfully saved
bool saveScene(const World& world, const std::string& filename);

// returns true if successfully loaded
bool loadScene(World& world, ResourceManager& resourceManager, const std::string& filename);