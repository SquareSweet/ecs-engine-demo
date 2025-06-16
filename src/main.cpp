#include <iostream>
#include <thread>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/World.hpp"
#include "ResourceManager.hpp"
#include "system/RenderSystem.hpp"
#include "system/ScriptingSystem.hpp"
#include "serialization/Serialization.hpp"

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ECS-based Engine Demo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);

    World world;
    ResourceManager resourceManager;

    Entity e1 = world.createEntity();

    TransformComponent tc1;
    tc1.position = {0.0f, -1.0f, 0.0f};
    tc1.rotation = {0.0f, 0.0f, 0.0f};
    tc1.scale    = {0.05f, 0.05f, 0.05f};
    world.addComponent(e1, tc1);

    RenderComponent rc1;
    rc1.modelPath = "assets/models/rat.obj";
    rc1.model = resourceManager.loadModel(rc1.modelPath);
    
    world.addComponent(e1, rc1);

    LuaScriptComponent sc1;
    sc1.scriptPath = "scripts/rotate.lua";
    world.addComponent(e1, sc1);

    RenderSystem renderSystem(&world, &resourceManager);
    ScriptingSystem scriptingSystem(&world);
    scriptingSystem.init();

    const float dt = 0.016f;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        scriptingSystem.update(dt);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        renderSystem.setViewportSize(w, h);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderSystem.render();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    // //serialization demo
    // world.clear();
    // {
    //     Entity e = world.createEntity();
    //     TransformComponent t; t.position = {2.0f, 0.0f, 0.0f};
    //     world.addComponent(e, t);
    //     RenderComponent r; r.modelPath = "assets/models/example.obj"; r.model = resourceManager.loadModel(r.modelPath);
    //     world.addComponent(e, r);
    // }
    // saveScene(world, "scene.json");

    // World newWorld;
    // ResourceManager newRM;
    // if (loadScene(newWorld, newRM, "scene.json")) {
    //     RenderSystem rs2(&newWorld, &newRM);
    //     std::cout << "Rendering loaded scene:\n";
    //     rs2.render();
    // }

    return 0;

    
}
