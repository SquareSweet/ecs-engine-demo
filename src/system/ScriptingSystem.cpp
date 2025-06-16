#include "system/ScriptingSystem.hpp"
#include <cmath>
#include <iostream>

// Lua global variables:
// "world_ptr" — lightuserdata pointiong to World*
// "entity_id" — current entity
// "dt" — delta time

ScriptingSystem::ScriptingSystem(World *world) : world(world) {
  L = luaL_newstate();
  if (!L) {
    std::cerr << "Failed to create Lua state" << std::endl;
    return;
  }
  luaL_openlibs(L);
  // Saving World pointer into global variable
  lua_pushlightuserdata(L, static_cast<void *>(world));
  lua_setglobal(L, "world_ptr");

  registerFunctions();
}

ScriptingSystem::~ScriptingSystem() {
  if (L) {
    lua_close(L);
  }
}

void ScriptingSystem::init() {
  // General scripts
}

void ScriptingSystem::update(float dt) {
  if (!L)
    return;
  // For every Entity with LuaScriptComponent:
  for (Entity e : world->getEntities()) {
    auto sc = world->getScript(e);
    if (sc) {
      // Check if loaded
      std::string loadedFlag = "script_" + std::to_string(e) + "_loaded";
      lua_getglobal(L, loadedFlag.c_str());
      bool isLoaded = lua_toboolean(L, -1);
      lua_pop(L, 1);
      if (!isLoaded) {
        // Load
        int status = luaL_dofile(L, sc->scriptPath.c_str());
        if (status != LUA_OK) {
          const char *msg = lua_tostring(L, -1);
          std::cerr << "Lua load error for entity " << e << ": "
                    << (msg ? msg : "unknown") << std::endl;
          lua_pop(L, 1);
        } else {
          // Mark loaded
          lua_pushboolean(L, 1);
          lua_setglobal(L, loadedFlag.c_str());
        }
      }
      callLuaUpdate(e, dt);
    }
  }
}

void ScriptingSystem::registerFunctions() {
  // Register global functions
  lua_register(L, "get_position", l_get_position);
  lua_register(L, "set_position", l_set_position);
  lua_register(L, "rotate", l_rotate);
}

// Get World*from global var
World *ScriptingSystem::getWorldFromLua(lua_State *L) {
  lua_getglobal(L, "world_ptr");
  World *w = static_cast<World *>(lua_touserdata(L, -1));
  lua_pop(L, 1);
  return w;
}

// Get current Entity from global var
Entity ScriptingSystem::getCurrentEntity(lua_State *L) {
  lua_getglobal(L, "entity_id");
  if (!lua_isinteger(L, -1)) {
    lua_pop(L, 1);
    return INVALID_ENTITY;
  }
  Entity e = static_cast<Entity>(lua_tointeger(L, -1));
  lua_pop(L, 1);
  return e;
}

// Lua: get_position(): returns table {x, y, z}
int ScriptingSystem::l_get_position(lua_State *L) {
  World *w = getWorldFromLua(L);
  Entity e = getCurrentEntity(L);
  if (w && e != INVALID_ENTITY) {
    TransformComponent *tc = w->getTransform(e);
    if (tc) {
      lua_newtable(L);
      lua_pushnumber(L, tc->position[0]);
      lua_rawseti(L, -2, 1);
      lua_pushnumber(L, tc->position[1]);
      lua_rawseti(L, -2, 2);
      lua_pushnumber(L, tc->position[2]);
      lua_rawseti(L, -2, 3);
      return 1;
    }
  }
  // if no transform or error return nil
  lua_pushnil(L);
  return 1;
}

// Lua: set_position(x, y, z)
int ScriptingSystem::l_set_position(lua_State *L) {
  World *w = getWorldFromLua(L);
  Entity e = getCurrentEntity(L);
  if (w && e != INVALID_ENTITY) {
    TransformComponent *tc = w->getTransform(e);
    if (tc) {
      // expecting 3 args
      if (lua_gettop(L) >= 3 && lua_isnumber(L, 1) && lua_isnumber(L, 2) &&
          lua_isnumber(L, 3)) {
        tc->position[0] = static_cast<float>(lua_tonumber(L, 1));
        tc->position[1] = static_cast<float>(lua_tonumber(L, 2));
        tc->position[2] = static_cast<float>(lua_tonumber(L, 3));
      } else {
        std::cerr << "set_position: invalid arguments" << std::endl;
      }
    }
  }
  return 0;
}

// Lua: rotate(axisTable, angle)
// axisTable: table {x,y,z}, angle: deg
int ScriptingSystem::l_rotate(lua_State *L) {
  World *w = getWorldFromLua(L);
  Entity e = getCurrentEntity(L);
  if (w && e != INVALID_ENTITY) {
    TransformComponent *tc = w->getTransform(e);
    if (tc) {
      // Expectiong table and number
      if (lua_gettop(L) >= 2 && lua_istable(L, 1) && lua_isnumber(L, 2)) {
        lua_rawgeti(L, 1, 1);
        lua_rawgeti(L, 1, 2);
        lua_rawgeti(L, 1, 3);
        if (lua_isnumber(L, -3) && lua_isnumber(L, -2) && lua_isnumber(L, -1)) {
          float ax = static_cast<float>(lua_tonumber(L, -3));
          float ay = static_cast<float>(lua_tonumber(L, -2));
          float az = static_cast<float>(lua_tonumber(L, -1));
          float angle = static_cast<float>(lua_tonumber(L, 2));
          // For simplicity: if axis matches with X/Y/Z (1,0,0) or (0,1,0) or
          // (0,0,1), just add angle to rotation.
          if (fabs(ax - 1.0f) < 1e-3f && fabs(ay) < 1e-3f && fabs(az) < 1e-3f) {
            tc->rotation[0] += angle;
          } else if (fabs(ay - 1.0f) < 1e-3f && fabs(ax) < 1e-3f &&
                     fabs(az) < 1e-3f) {
            tc->rotation[1] += angle;
          } else if (fabs(az - 1.0f) < 1e-3f && fabs(ax) < 1e-3f &&
                     fabs(ay) < 1e-3f) {
            tc->rotation[2] += angle;
          } else {
            // Complex axis: add proportionally
            tc->rotation[0] += ax * angle;
            tc->rotation[1] += ay * angle;
            tc->rotation[2] += az * angle;
          }
        }
        lua_pop(L, 3); // clear axis vals
      } else {
        std::cerr << "rotate: invalid arguments" << std::endl;
      }
    }
  }
  return 0;
}

// Calls Lua update() for Entuty e, passing dt via global var.
void ScriptingSystem::callLuaUpdate(Entity e, float dt) {
  // Setting entity_id and dt
  lua_pushinteger(L, static_cast<lua_Integer>(e));
  lua_setglobal(L, "entity_id");
  lua_pushnumber(L, dt);
  lua_setglobal(L, "dt");

  lua_getglobal(L, "update");
  if (lua_isfunction(L, -1)) {
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
      const char *msg = lua_tostring(L, -1);
      std::cerr << "Lua runtime error in update for entity " << e << ": "
                << (msg ? msg : "unknown") << std::endl;
      lua_pop(L, 1);
    }
  } else {
    lua_pop(L, 1);
  }
}
