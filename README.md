# Winter Rain ECS

![alt text](https://github.com/GabrielBernardoDaSilva/forged_in_lost_lands_ecs/blob/main/winter-rain.png)

Winter Rain is an ECS (Entity-Component-System) library developed in C++23. It provides a powerful framework for building scalable and efficient game engines and simulations.

## Features

- **Entity Creation**: Easily create entities and attach components to them.
- **Event Handling**: Implement event-driven architecture with custom events and event handlers.
- **Schedulers**: Schedule tasks to be executed at specific intervals or delays.
- **Queries**: Efficiently query entities based on their components.
- **Systems**: Implement game logic and behaviors through systems that process entities based on queries.
- **Plugins**: Extend the functionality of the ECS with custom plugins.

## Requirements

- C++23 Compiler(clang)

## Example Usage

```cpp
#include <print>

#include "winter_rain_ecs.hpp"

using namespace winter_rain_ecs;

struct Velocity : public Component
{
    float x, y;
};

struct Health : public Component
{
    int health;
};

struct Position : public Component
{
    float x, y;
};

generator<WaitAmountOfSeconds> generate_numbers(int i)
{
    std::println("generate_numbers starting");
    co_yield WaitAmountOfSeconds{
        .seconds = 100000.0f};
    std::println("generate_numbers ending");

    std::println("i: {}", i);
    std::exit(0);
}

struct Collision : public Event
{
    bool collided;
};

void check_collision(Collision collision)
{
    std::println("Collision: {}", collision.collided);
}

void modify_pos(Query<With<Position &, Velocity &>> query, Query<With<Position &>, Without<Velocity &>> q2, EventManager &event_manager, EntityManager &entity_manager)
{
    entity_manager.add_entity(Position{.x = 100.0f, .y = 100.0f}, Velocity{.x = 100.0f, .y = 100.0f});
    event_manager.subscribe<Collision>(check_collision);
    auto a = query.all();
    auto b = q2.all();
    for (auto &[pos, vel] : a)
    {
        std::println("Position: x: {} y: {}", pos.x, pos.y);
        pos.x += 1.0f;
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
    virtual void build(LostLands &lost_lands) override
    {
        std::println("PluginTest on_start");
    }
};

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

    LostLands lands;
    Entity e = lands.add_entity(pos, vel);
    Entity e2 = lands.add_entity(pos2, vel2);
    Entity e3 = lands.add_entity(pos3, health);

    lands.add_component_to_entity(e, Health{.health = 100});
    lands.remove_component_from_entity<Health>(e);

    lands.add_executors(ExecutorType::Startup, modify_pos, read_position);
    lands.add_executors(ExecutorType::Startup, pos_query);

    lands.run();
    lands.emit(Collision{.collided = true});

    lands.add_task(generate_numbers, 10);

    lands.add_plugin<PluginTest>();
    lands.build_plugins();

    while (true)
    {

        lands.run_tasks(0.16f);
    }

    return 0;
}

```

This README provides an overview of the library's features, example usage, requirements, installation instructions, and licensing information. Adjustments can be made as needed based on the specific details of the library.

## License

Winter Rain is licensed under the MIT License. See LICENSE for details.
