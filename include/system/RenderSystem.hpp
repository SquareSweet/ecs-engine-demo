#pragma once
#include "../ResourceManager.hpp"
#include "../core/World.hpp"
#include "Shader.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Iterates through Entities with TransformComponent or RenderComponent.
class RenderSystem {
public:
  RenderSystem(World *world, ResourceManager *rm)
      : world(world), resourceManager(rm) {
    std::string vertSrc = readFile("shaders/basic.vert");
    std::string fragSrc = readFile("shaders/basic.frag");
    shader = std::make_unique<Shader>(vertSrc, fragSrc);
  }

  void setViewportSize(int w, int h) {
    screenWidth = w;
    screenHeight = h;
  }

  void render() {
    if (screenWidth == 0 || screenHeight == 0)
      return;
    // fixed camera
    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), -camPos);
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f,
        100.0f);

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVec3("lightPos", glm::vec3(0.0f, 5.0f, 5.0f));
    shader->setVec3("lightColor", glm::vec3(1.0f));
    shader->setVec3("viewPos", camPos);

    for (Entity e : world->getEntities()) {
      auto tc = world->getTransform(e);
      auto rc = world->getRender(e);
      if (tc && rc && rc->model) {
        Model *model = rc->model.get();
        if (!model->uploadedToGPU) {
          uploadModelToGPU(model);
        }
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat =
            glm::translate(modelMat, glm::vec3(tc->position[0], tc->position[1],
                                               tc->position[2]));
        modelMat = glm::rotate(modelMat, glm::radians(tc->rotation[0]),
                               glm::vec3(1, 0, 0));
        modelMat = glm::rotate(modelMat, glm::radians(tc->rotation[1]),
                               glm::vec3(0, 1, 0));
        modelMat = glm::rotate(modelMat, glm::radians(tc->rotation[2]),
                               glm::vec3(0, 0, 1));
        modelMat = glm::scale(
            modelMat, glm::vec3(tc->scale[0], tc->scale[1], tc->scale[2]));

        shader->setMat4("model", modelMat);

        shader->setBool("useTexture", false);
        shader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));

        glBindVertexArray(model->VAO);
        GLsizei indexCount = static_cast<GLsizei>(model->indices.size());
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
      }
    }
  }

private:
  World *world;
  ResourceManager *resourceManager;
  int screenWidth = 800, screenHeight = 600;
  std::unique_ptr<Shader> shader;

  void uploadModelToGPU(Model *model) {
    bool hasNormals = !model->normals.empty();
    bool hasTexcoords = !model->texcoords.empty();
    size_t vertCount = model->positions.size() / 3;
    std::vector<float> vertexData;
    vertexData.reserve(vertCount *
                       (3 + (hasNormals ? 3 : 0) + (hasTexcoords ? 2 : 0)));

    for (size_t i = 0; i < vertCount; ++i) {
      vertexData.push_back(model->positions[3 * i + 0]);
      vertexData.push_back(model->positions[3 * i + 1]);
      vertexData.push_back(model->positions[3 * i + 2]);
      if (hasNormals) {
        vertexData.push_back(model->normals[3 * i + 0]);
        vertexData.push_back(model->normals[3 * i + 1]);
        vertexData.push_back(model->normals[3 * i + 2]);
      }
      if (hasTexcoords) {
        vertexData.push_back(model->texcoords[2 * i + 0]);
        vertexData.push_back(model->texcoords[2 * i + 1]);
      }
    }

    glGenVertexArrays(1, &model->VAO);
    glGenBuffers(1, &model->VBO);
    glGenBuffers(1, &model->EBO);

    glBindVertexArray(model->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float),
                 vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 model->indices.size() * sizeof(unsigned int),
                 model->indices.data(), GL_STATIC_DRAW);

    // location = 0 : position (vec3)
    // location = 1 : normal   (vec3)
    // location = 2 : texcoord (vec2)
    GLsizei stride =
        (GLsizei)((3 + (hasNormals ? 3 : 0) + (hasTexcoords ? 2 : 0)) *
                  sizeof(float));
    size_t offset = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)(offset));
    offset += 3 * sizeof(float);
    if (hasNormals) {
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(offset));
      offset += 3 * sizeof(float);
    }
    if (hasTexcoords) {
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(offset));
      offset += 2 * sizeof(float);
    }

    glBindVertexArray(0);
    model->uploadedToGPU = true;
  }

  std::string readFile(const std::string &path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
      std::cerr << "Failed to open file: " << path << std::endl;
      return "";
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
  }
};
