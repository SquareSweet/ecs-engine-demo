#pragma once

#include "Component.hpp"
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <iostream>

// In real graphic engine would contain, UV, GPU buffers, etc.
struct Model {
    struct Vertex {
        float x, y, z;
    };
    std::vector<Vertex> vertices;
    std::vector<std::array<int, 3>> faces; // 0-based
};

// Shaders, textures, etc
struct Material {
    // Not used in demo
};

struct RenderComponent : Component {
    std::shared_ptr<Model> model;
    std::shared_ptr<Material> material;
    std::string modelPath; // for serialization
};