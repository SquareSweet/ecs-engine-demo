#pragma once
#include "../core/World.hpp"
#include "../ResourceManager.hpp"

// Iterates through Entities with TransformComponent or RenderComponent.
// Console output inctead of rendering
// TODO: Intrgrate graphics API (OpenGL/DirectX/Vulkan и т.д.).
class RenderSystem {
public:
    RenderSystem(World* world, ResourceManager* rm)
        : world(world), resourceManager(rm) {}

    void render() {
        // Setting up camera, shaders, etc
        for (Entity e : world->getEntities()) {
            auto tc = world->getTransform(e);
            auto rc = world->getRender(e);
            if (tc && rc && rc->model) {
                // Graphic API integration:
                // resourceManager.loadModel(...) already loaded data to memory,
                // here should be using GPU-buffers, shaders and draw calls.
                // Using console output instead:
                printf("Entity %u: position=(%.2f, %.2f, %.2f), rotation=(%.2f, %.2f, %.2f), scale=(%.2f, %.2f, %.2f), model='%s'\n",
                    e,
                    tc->position[0], tc->position[1], tc->position[2],
                    tc->rotation[0], tc->rotation[1], tc->rotation[2],
                    tc->scale[0], tc->scale[1], tc->scale[2],
                    rc->modelPath.c_str()
                );
            }
        }
    }

private:
    World* world;
    ResourceManager* resourceManager;
};
