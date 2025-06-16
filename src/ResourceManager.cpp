#include "ResourceManager.hpp"
#include <filesystem>
#include <glm/glm.hpp>
#include <iostream>
#include <tiny_obj_loader.h>

std::shared_ptr<Model> ResourceManager::loadModel(const std::string &path) {
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
            << " (positions: " << modelPtr->positions.size() / 3
            << ", indices: " << modelPtr->indices.size() << ")" << std::endl;
  return modelPtr;
}

bool ResourceManager::parseOBJ(const std::string &path, Model &outModel) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn;
  std::string err;

  std::string basedir;
  {
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
      basedir = p.parent_path().string() + "/";
    } else {
      basedir = "";
    }
  }

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(),
                              basedir.empty() ? nullptr : basedir.c_str(),
                              /*triangulate=*/true);
  if (!warn.empty()) {
    std::cerr << "tinyobjloader warning: " << warn << std::endl;
  }
  if (!err.empty()) {
    std::cerr << "tinyobjloader error: " << err << std::endl;
  }
  if (!ret) {
    std::cerr << "Failed to load/parse .obj: " << path << std::endl;
    return false;
  }

  outModel.positions.clear();
  outModel.normals.clear();
  outModel.texcoords.clear();
  outModel.indices.clear();
  outModel.materials.clear();

  outModel.materials.reserve(materials.size());
  for (const auto &mat : materials) {
    Model::MaterialInfo mi;
    mi.name = mat.name;
    mi.diffuse_color =
        glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
    mi.ambient_color =
        glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
    mi.specular_color =
        glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
    mi.shininess = mat.shininess;
    mi.diffuse_texname = mat.diffuse_texname;
    mi.ambient_texname = mat.ambient_texname;
    mi.specular_texname = mat.specular_texname;
    outModel.materials.push_back(std::move(mi));
  }

  struct VertexKey {
    int vi, ti, ni;
    bool operator==(VertexKey const &o) const {
      return vi == o.vi && ti == o.ti && ni == o.ni;
    }
  };
  struct VertexKeyHasher {
    std::size_t operator()(VertexKey const &k) const noexcept {
      return (static_cast<size_t>(k.vi) * 73856093) ^
             (static_cast<size_t>(k.ti) * 19349663) ^
             (static_cast<size_t>(k.ni) * 83492791);
    }
  };
  std::unordered_map<VertexKey, unsigned int, VertexKeyHasher> uniqueVertexMap;
  uniqueVertexMap.reserve(1000);

  size_t estimatedIndices = 0;
  for (const auto &shape : shapes) {
    estimatedIndices += shape.mesh.indices.size();
  }
  outModel.indices.reserve(estimatedIndices);

  outModel.positions.reserve(attrib.vertices.size());
  if (!attrib.normals.empty())
    outModel.normals.reserve(attrib.normals.size());
  if (!attrib.texcoords.empty())
    outModel.texcoords.reserve(attrib.texcoords.size());

  bool hasNormals = !attrib.normals.empty();
  bool hasTexcoords = !attrib.texcoords.empty();

  for (const auto &shape : shapes) {
    const auto &mesh = shape.mesh;
    for (size_t idx = 0; idx < mesh.indices.size(); ++idx) {
      tinyobj::index_t idx0 = mesh.indices[idx];
      VertexKey key{idx0.vertex_index, idx0.texcoord_index, idx0.normal_index};
      auto it = uniqueVertexMap.find(key);
      if (it != uniqueVertexMap.end()) {
        outModel.indices.push_back(it->second);
      } else {
        unsigned int newIndex =
            static_cast<unsigned int>(outModel.positions.size() / 3);
        uniqueVertexMap[key] = newIndex;
        int v = idx0.vertex_index * 3;
        outModel.positions.push_back(attrib.vertices[v + 0]);
        outModel.positions.push_back(attrib.vertices[v + 1]);
        outModel.positions.push_back(attrib.vertices[v + 2]);
        if (hasTexcoords && idx0.texcoord_index >= 0) {
          int t = idx0.texcoord_index * 2;
          outModel.texcoords.push_back(attrib.texcoords[t + 0]);
          outModel.texcoords.push_back(attrib.texcoords[t + 1]);
        } else {
          outModel.texcoords.push_back(0.0f);
          outModel.texcoords.push_back(0.0f);
        }
        if (hasNormals && idx0.normal_index >= 0) {
          int n = idx0.normal_index * 3;
          outModel.normals.push_back(attrib.normals[n + 0]);
          outModel.normals.push_back(attrib.normals[n + 1]);
          outModel.normals.push_back(attrib.normals[n + 2]);
        } else {
          outModel.normals.push_back(0.0f);
          outModel.normals.push_back(0.0f);
          outModel.normals.push_back(0.0f);
        }
        outModel.indices.push_back(newIndex);
      }
    }
  }

  if (!hasNormals) {
    size_t vertCount = outModel.positions.size() / 3;
    outModel.normals.assign(vertCount * 3, 0.0f);
    size_t faceCount = outModel.indices.size() / 3;
    for (size_t f = 0; f < faceCount; ++f) {
      unsigned int i0 = outModel.indices[3 * f + 0];
      unsigned int i1 = outModel.indices[3 * f + 1];
      unsigned int i2 = outModel.indices[3 * f + 2];
      glm::vec3 v0(outModel.positions[3 * i0 + 0],
                   outModel.positions[3 * i0 + 1],
                   outModel.positions[3 * i0 + 2]);
      glm::vec3 v1(outModel.positions[3 * i1 + 0],
                   outModel.positions[3 * i1 + 1],
                   outModel.positions[3 * i1 + 2]);
      glm::vec3 v2(outModel.positions[3 * i2 + 0],
                   outModel.positions[3 * i2 + 1],
                   outModel.positions[3 * i2 + 2]);
      glm::vec3 edge1 = v1 - v0;
      glm::vec3 edge2 = v2 - v0;
      glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
      for (int vi : {(int)i0, (int)i1, (int)i2}) {
        outModel.normals[3 * vi + 0] += faceNormal.x;
        outModel.normals[3 * vi + 1] += faceNormal.y;
        outModel.normals[3 * vi + 2] += faceNormal.z;
      }
    }
    for (size_t vi = 0; vi < vertCount; ++vi) {
      glm::vec3 n(outModel.normals[3 * vi + 0], outModel.normals[3 * vi + 1],
                  outModel.normals[3 * vi + 2]);
      n = glm::normalize(n);
      outModel.normals[3 * vi + 0] = n.x;
      outModel.normals[3 * vi + 1] = n.y;
      outModel.normals[3 * vi + 2] = n.z;
    }
  }

  return true;
}
