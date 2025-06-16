#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
  Shader(const std::string &vertexSrc, const std::string &fragmentSrc);
  ~Shader();

  void use() const;
  unsigned int getID() const { return ID; }

  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMat4(const std::string &name, const glm::mat4 &mat) const;
  void setVec3(const std::string &name, const glm::vec3 &vec) const;

private:
  unsigned int ID;

  void checkCompileErrors(unsigned int shader, const std::string &type) const;
};
