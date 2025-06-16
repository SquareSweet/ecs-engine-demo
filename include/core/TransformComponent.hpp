#pragma once

#include "Component.hpp"
#include <array>

// std::array<float,3> for simplisity.
struct TransformComponent : Component {
    std::array<float, 3> position{0.0f, 0.0f, 0.0f};
    std::array<float, 3> rotation{0.0f, 0.0f, 0.0f}; // rotation around X, Y, Z in deg
    std::array<float, 3> scale{1.0f, 1.0f, 1.0f};
};