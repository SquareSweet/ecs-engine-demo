#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include "Entity.hpp"
#include "TransformComponent.hpp"
#include "RenderComponent.hpp"
#include "LuaScriptComponent.hpp"

// Manages Entities and Components
// Separate std::unordered_map<Entity, ComponentType> for every component type
// TODO: deletion
class World {
public:
    World() = default;

    Entity createEntity() {
        Entity id = nextEntityId++;
        entities.push_back(id);
        return id;
    }

    const std::vector<Entity>& getEntities() const {
        return entities;
    }

    void addComponent(Entity e, const TransformComponent& comp) {
        transforms[e] = comp;
    }
    void addComponent(Entity e, const RenderComponent& comp) {
        renders[e] = comp;
    }
    void addComponent(Entity e, const LuaScriptComponent& comp) {
        scripts[e] = comp;
    }

    bool hasTransform(Entity e) const {
        return transforms.find(e) != transforms.end();
    }
    bool hasRender(Entity e) const {
        return renders.find(e) != renders.end();
    }
    bool hasScript(Entity e) const {
        return scripts.find(e) != scripts.end();
    }

    TransformComponent* getTransform(Entity e) {
        auto it = transforms.find(e);
        if (it != transforms.end()) return &it->second;
        return nullptr;
    }
    RenderComponent* getRender(Entity e) {
        auto it = renders.find(e);
        if (it != renders.end()) return &it->second;
        return nullptr;
    }
    LuaScriptComponent* getScript(Entity e) {
        auto it = scripts.find(e);
        if (it != scripts.end()) return &it->second;
        return nullptr;
    }
    const TransformComponent* getTransform(Entity e) const {
        auto it = transforms.find(e);
        if (it != transforms.end()) return &it->second;
        return nullptr;
    }
    const RenderComponent* getRender(Entity e) const {
        auto it = renders.find(e);
        if (it != renders.end()) return &it->second;
        return nullptr;
    }
    const LuaScriptComponent* getScript(Entity e) const {
        auto it = scripts.find(e);
        if (it != scripts.end()) return &it->second;
        return nullptr;
    }

    // For serialization
    const std::unordered_map<Entity, TransformComponent>& getAllTransforms() const {
        return transforms;
    }
    const std::unordered_map<Entity, RenderComponent>& getAllRenders() const {
        return renders;
    }
    const std::unordered_map<Entity, LuaScriptComponent>& getAllScripts() const {
        return scripts;
    }

    void clear() {
        entities.clear();
        transforms.clear();
        renders.clear();
        scripts.clear();
        nextEntityId = 1;
    }

private:
    Entity nextEntityId = 1;
    std::vector<Entity> entities;

    std::unordered_map<Entity, TransformComponent> transforms;
    std::unordered_map<Entity, RenderComponent> renders;
    std::unordered_map<Entity, LuaScriptComponent> scripts;
};