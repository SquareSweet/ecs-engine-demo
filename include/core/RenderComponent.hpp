#pragma once

#include "Component.hpp"
#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Model {
  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<float> texcoords;
  std::vector<unsigned int> indices;

  struct MaterialInfo {
    std::string name;
    std::string diffuse_texname;
    std::string ambient_texname;
    std::string specular_texname;
    glm::vec3 diffuse_color = glm::vec3(1.0f);
    glm::vec3 ambient_color = glm::vec3(1.0f);
    glm::vec3 specular_color = glm::vec3(1.0f);
    float shininess = 1.0f;
    // GLuint diffuseTexID = 0; //
  };
  std::vector<MaterialInfo> materials;

  unsigned int VAO = 0;
  unsigned int VBO = 0;
  unsigned int EBO = 0;
  bool uploadedToGPU = false;
};

struct RenderComponent : Component {
  std::shared_ptr<Model> model;
  std::string modelPath; // for serialization
};