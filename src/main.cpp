#include <iostream>
#include <print>
#include <type_traits>
#include <tuple>
#include <string_view>
#include <coroutine>

#include "lost_lands.hpp"
#include "component.hpp"
#include "archetype.hpp"
#include "query.hpp"
#include "system.hpp"
#include "forged_concepts.hpp"
#include "executor.hpp"
#include "event.hpp"
#include "scheduler.hpp"
#include "plugin.hpp"

using namespace forged_in_lost_lands_ecs;

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
    std::cout << "generate_numbers starting" << std::endl;
    co_yield WaitAmountOfSeconds{
        .seconds = 100000.0f};
    std::cout << "generate_numbers ending" << std::endl;

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

void modifing_pos(Query<With<Position &, Velocity &>> query, Query<With<Position &>, Without<Velocity &>> q2, EventManager &event_manager, EntityManager &entity_manager)
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
    lands.show_archetypes();
    lands.remove_component_from_entity<Health>(e);
    lands.show_archetypes();

    lands.add_executor(modifing_pos);
    lands.add_executor(read_position);

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
