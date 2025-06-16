#include "serialization/Serialization.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool saveScene(const World& world, const std::string& filename) {
    json jScene;
    jScene["entities"] = json::array();

    for (Entity e : world.getEntities()) {
        json jEntity;
        jEntity["id"] = e;

        if (world.hasTransform(e)) {
            const auto* tc = world.getTransform(e);
            json jTrans;
            jTrans["position"] = { tc->position[0], tc->position[1], tc->position[2] };
            jTrans["rotation"] = { tc->rotation[0], tc->rotation[1], tc->rotation[2] };
            jTrans["scale"] = { tc->scale[0], tc->scale[1], tc->scale[2] };
            jEntity["TransformComponent"] = jTrans;
        }
        if (world.hasRender(e)) {
            const auto* rc = world.getRender(e);
            json jRender;
            jRender["modelPath"] = rc->modelPath;
            jEntity["RenderComponent"] = jRender;
        }
        if (world.hasScript(e)) {
            const auto* sc = world.getScript(e);
            json jScript;
            jScript["scriptPath"] = sc->scriptPath;
            jEntity["LuaScriptComponent"] = jScript;
        }
        jScene["entities"].push_back(jEntity);
    }

    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Cannot open file for saving scene: " << filename << std::endl;
        return false;
    }
    ofs << jScene.dump(4);
    ofs.close();
    std::cout << "Scene saved to " << filename << std::endl;
    return true;
}

bool loadScene(World& world, ResourceManager& resourceManager, const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Cannot open scene file: " << filename << std::endl;
        return false;
    }
    json jScene;
    try {
        ifs >> jScene;
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error in " << filename << ": " << e.what() << std::endl;
        return false;
    }

    if (!jScene.contains("entities") || !jScene["entities"].is_array()) {
        std::cerr << "Invalid scene format: missing 'entities' array" << std::endl;
        return false;
    }

    // Empty World implied
    for (const auto& jEntity : jScene["entities"]) {
        if (!jEntity.contains("id") || !jEntity["id"].is_number_unsigned()) {
            std::cerr << "Entity without valid 'id' field" << std::endl;
            continue;
        }
        Entity e = static_cast<Entity>(jEntity["id"].get<uint32_t>());

        // Creating new Entity without saving ID.
        // Could be reworked with saving IDs if entities interactions needed
        Entity newE = world.createEntity();

        if (jEntity.contains("TransformComponent")) {
            const auto& jTrans = jEntity["TransformComponent"];
            TransformComponent tc;
            auto pos = jTrans["position"];
            tc.position = { pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>() };
            auto rot = jTrans["rotation"];
            tc.rotation = { rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>() };
            auto scl = jTrans["scale"];
            tc.scale = { scl[0].get<float>(), scl[1].get<float>(), scl[2].get<float>() };
            world.addComponent(newE, tc);
        }
        if (jEntity.contains("RenderComponent")) {
            const auto& jRender = jEntity["RenderComponent"];
            RenderComponent rc;
            rc.modelPath = jRender["modelPath"].get<std::string>();
            rc.model = resourceManager.loadModel(rc.modelPath);
            // For materials
            world.addComponent(newE, rc);
        }
        if (jEntity.contains("LuaScriptComponent")) {
            const auto& jScript = jEntity["LuaScriptComponent"];
            LuaScriptComponent sc;
            sc.scriptPath = jScript["scriptPath"].get<std::string>();
            world.addComponent(newE, sc);
        }
    }

    std::cout << "Scene loaded from " << filename << std::endl;
    return true;
}
