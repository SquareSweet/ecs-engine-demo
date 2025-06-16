#pragma once

#include "ResourceManager.hpp"
#include "core/World.hpp"
#include <string>

// returns true if successfully saved
bool saveScene(const World &world, const std::string &filename);

// returns true if successfully loaded
bool loadScene(World &world, ResourceManager &resourceManager,
               const std::string &filename);