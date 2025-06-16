#include <iostream>
#include <thread>
#include <chrono>
#include "core/World.hpp"
#include "ResourceManager.hpp"
#include "system/RenderSystem.hpp"
#include "system/ScriptingSystem.hpp"
#include "serialization/Serialization.hpp"

int main() {
    World world;
    ResourceManager resourceManager;

    Entity e1 = world.createEntity();

    TransformComponent tc1;
    tc1.position = {0.0f, 0.0f, 0.0f};
    tc1.rotation = {0.0f, 0.0f, 0.0f};
    tc1.scale    = {1.0f, 1.0f, 1.0f};
    world.addComponent(e1, tc1);

    RenderComponent rc1;
    rc1.modelPath = "assets/models/example.obj";
    rc1.model = resourceManager.loadModel(rc1.modelPath);
    rc1.material = std::make_shared<Material>();
    world.addComponent(e1, rc1);

    LuaScriptComponent sc1;
    sc1.scriptPath = "scripts/rotate.lua";
    world.addComponent(e1, sc1);

    RenderSystem renderSystem(&world, &resourceManager);
    ScriptingSystem scriptingSystem(&world);
    scriptingSystem.init();

    const int numFrames = 5;
    const float dt = 0.016f; // ~60 FPS

    std::cout << "Starting main loop, running " << numFrames << " frames...\n";
    for (int i = 0; i < numFrames; ++i) {
        scriptingSystem.update(dt);

        // console output
        renderSystem.render();

        // imitating render cycles
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    //serialization demo
    world.clear();
    {
        Entity e = world.createEntity();
        TransformComponent t; t.position = {2.0f, 0.0f, 0.0f};
        world.addComponent(e, t);
        RenderComponent r; r.modelPath = "assets/models/example.obj"; r.model = resourceManager.loadModel(r.modelPath);
        world.addComponent(e, r);
    }
    saveScene(world, "scene.json");

    World newWorld;
    ResourceManager newRM;
    if (loadScene(newWorld, newRM, "scene.json")) {
        RenderSystem rs2(&newWorld, &newRM);
        std::cout << "Rendering loaded scene:\n";
        rs2.render();
    }

    std::cout << "Finished.\n";
    return 0;
}
