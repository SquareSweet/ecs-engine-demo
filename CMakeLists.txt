cmake_minimum_required(VERSION 3.15)
project(ecs_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# nlohmann/json
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES src/*.cpp)

# Lua
find_package(Lua 5.3 REQUIRED)
if (LUA_FOUND)
    message(STATUS "Lua include dirs: ${LUA_INCLUDE_DIR}")
    message(STATUS "Lua libraries: ${LUA_LIBRARIES}")
else()
    message(FATAL_ERROR "Lua not found. Install Lua devkit (liblua5.3-dev for Linux).")
endif()

add_executable(ecs_demo ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(ecs_demo PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(ecs_demo PRIVATE ${LUA_LIBRARIES})