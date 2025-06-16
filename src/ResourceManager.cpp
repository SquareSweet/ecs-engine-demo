#include "ResourceManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

std::shared_ptr<Model> ResourceManager::loadModel(const std::string& path) {
    auto it = models.find(path);
    if (it != models.end()) {
        return it->second;
    }

    auto modelPtr = std::make_shared<Model>();
    if (!parseOBJ(path, *modelPtr)) {
        std::cerr << "Failed to load model from " << path << std::endl;
        return nullptr;
    }
    models[path] = modelPtr;
    std::cout << "Model loaded: " << path
              << " (vertices: " << modelPtr->vertices.size()
              << ", faces: " << modelPtr->faces.size() << ")" << std::endl;
    return modelPtr;
}

// Simple .obj parser, only processes strings like:
// v x y z
// f i j k
bool ResourceManager::parseOBJ(const std::string& path, Model& outModel) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Cannot open OBJ file: " << path << std::endl;
        return false;
    }
    std::string line;
    std::vector<Model::Vertex> tempVerts;
    while (std::getline(file, line)) {
        if (line.size() < 2) continue;
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            tempVerts.push_back({x, y, z});
        } else if (prefix == "f") {
            // Triangles: "f i j k"
            int vi[3];
            iss >> vi[0] >> vi[1] >> vi[2];
            if (iss.fail()) continue;
            // 1-based to 0-based
            outModel.faces.push_back({vi[0] - 1, vi[1] - 1, vi[2] - 1});
        }
        // Ignore everything else
    }
    outModel.vertices = std::move(tempVerts);
    return true;
}
