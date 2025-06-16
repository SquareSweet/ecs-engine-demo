#pragma once
#include "../core/World.hpp"
#include <lua.hpp>

// Lua Integration
// Creates one lua_State*, regirster function to manage TransformComponent.
// Every frame calls update() in Lua-script for every entity with
// LuaScriptComponent.
class ScriptingSystem {
public:
  ScriptingSystem(World *world);
  ~ScriptingSystem();

  void init();

  void update(float dt);

private:
  World *world;
  lua_State *L = nullptr;

  void registerFunctions();

  static int l_get_position(lua_State *L);
  static int l_set_position(lua_State *L);
  static int l_rotate(lua_State *L);

  static World *getWorldFromLua(lua_State *L);

  static Entity getCurrentEntity(lua_State *L);

  void callLuaUpdate(Entity e, float dt);
};
