# Dark River ECS
<p align="center">
      <img src="https://github.com/GabrielBernardoDaSilva/forged_in_lost_lands_ecs/blob/main/darkriver.png" alt="DARKRIVER" />
</p>

[badge.language]: https://img.shields.io/badge/language-C%2B%2B23-red

Dark River is an ECS (Entity-Component-System) library developed in C++23. It provides a powerful framework for building scalable and efficient game engines and simulations.

## Features

- **Entity Creation**: Easily create entities and attach components to them.
- **Event Handling**: Implement event-driven architecture with custom events and event handlers.
- **Schedulers**: Schedule tasks to be executed at specific intervals or delays.
- **Queries**: Efficiently query entities based on their components.
- **Systems**: Implement game logic and behaviors through systems that process entities based on queries.
- **Plugins**: Extend the functionality of the ECS with custom plugins.

## Requirements

- C++23
- [clang](https://clang.llvm.org/)
- [gcc](https://www.gnu.org/software/gcc/)

## Installation

You can add the root repository to your cmake:

```cmake
add_subdirectory(WinterRainECS)
target_link_libraries(${PROJECT_NAME} PRIVATE WinterRainECS)
```

## Example Usage

```cpp
#include <print>
#include <functional>

#include "darkriver.hpp"

using namespace darkriver;
struct Position
{
    float x, y;
};

struct Velocity
{
    float x, y;
};

struct Health
{
    int health;
};

generator<WaitAmountOfMilleSeconds> generate_numbers(int i)
{
    std::println("generate_numbers starting");
    co_yield WaitAmountOfMilleSeconds{
        .m_seconds = 1000.0f};
    std::println("generate_numbers ending");

    std::println("i: {}", i);
    std::exit(0);
}

struct Collision
{
    bool collided;
};

void check_collision(World &world, Collision collision)
{
    std::println("Collision: {}", collision.collided);
}

void modify_pos(Query<With<Entity &, Position &, Velocity &>> query, Query<With<Position &>, Without<Velocity &>> q2,
                EventManager &event_manager, EntityManager &entity_manager)
{
    entity_manager.add_entity(Position{.x = 200.0f, .y = 200.0f}, Velocity{.x = 200.0f, .y = 200.0f});
    event_manager.subscribe<Collision>(check_collision);
    auto a = query.all();
    auto b = q2.all();
    for (auto &[entity, pos, vel] : a)
    {
        std::println("Entity: {}", entity.id);
        std::println("Position: x: {} y: {}", pos.x, pos.y);
        pos.x += 1.0f;
        std::println("Velocity: x: {} y: {}", vel.x, vel.y);
    }

    for (auto &[pos] : b)
    {
        std::println("Pos Health: x: {} y: {}", pos.x, pos.y);
        pos.x += 1.0f;
    }
}

void read_position(Query<With<Position &>> query)
{
    for (auto &[pos] : query.all())
    {
        std::println("Position: x: {} y: {}", pos.x, pos.y);
    }
}

class PluginTest : public Plugin
{
    virtual void build(World &world) override
    {
        world.subscribe<Collision>([&](World &world, Collision collision)
                                   { std::println("PluginTest Collision"); });
        std::println("PluginTest on_start");
    }
};

void p(std::function<void()> f)
{
    f();
}

int main()
{
    Position pos = {
        .x = 10.0f,
        .y = 20.0f};
    Velocity vel = {
        .x = 5.0f,
        .y = 5.0f};

    Position pos2 = {
        .x = 12.0f,
        .y = 22.0f};

    Velocity vel2 = {
        .x = 7.0f,
        .y = 7.0f};

    Position pos3 = {
        .x = 13.0f,
        .y = 23.0f};
    Health health = {
        .health = 100};

    World world;
    world.add_plugin<PluginTest>();
    world.build_plugins();
    Entity e = world.add_entity(pos, vel);
    Entity e2 = world.add_entity(pos2, vel2);
    Entity e3 = world.add_entity(pos3, health);

    world.add_component_to_entity(e, Health{.health = 300});
    world.remove_component_from_entity<Health>(e);


    auto pos_query = [](Query<With<Position &>> query)
    {
        for (auto &[pos] : query.all())
        {
            std::println("Print Lambda Position: x: {} y: {}", pos.x, pos.y);
        }
    };

    world.add_executors(ExecutorType::Startup, modify_pos, read_position);
    world.add_executors(ExecutorType::Startup, pos_query);

    world.run();
    world.emit(Collision{.collided = true});

    world.add_task(generate_numbers, 10);


    auto f = []()
    {
        std::println("Hello");
    };
    p(f);

    std::println("Run");
    while (true)
    {
        world.run_tasks(0.16f);
    }
    return 0;
}

```

This README provides an overview of the library's features, example usage, requirements, installation instructions, and licensing information. Adjustments can be made as needed based on the specific details of the library.

## License

Dark River is licensed under the MIT License. See LICENSE for details.
