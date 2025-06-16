#pragma once

#include "Component.hpp"
#include <string>

// Just stores path
struct LuaScriptComponent : Component {
  std::string scriptPath;
};
