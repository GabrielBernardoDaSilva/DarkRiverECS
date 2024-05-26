
#include <print>
#include <functional>

#include "forged_in_lost_lands_ecs.hpp"

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
    virtual void build(World &lost_lands) override
    {
        std::println("PluginTest on_start");
    }
};
void p(std::function <void()> f)
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

    World lands;
    Entity e = lands.add_entity(pos, vel);
    Entity e2 = lands.add_entity(pos2, vel2);
    Entity e3 = lands.add_entity(pos3, health);

    lands.add_component_to_entity(e, Health{.health = 100});
    lands.show_archetypes();
    lands.remove_component_from_entity<Health>(e);
    lands.show_archetypes();

    // lands.add_executor(ExecutorType::Startup, modifing_pos);
    // lands.add_executor(ExecutorType::Startup, read_position);

    auto pos_query = [](Query<With<Position &>> query) {
        for (auto &[pos] : query.all())
        {
            std::println("Print Lambda Position: x: {} y: {}", pos.x, pos.y);
        }
    };

    auto &a = modifing_pos;

    lands.add_executors(ExecutorType::Startup, modifing_pos, read_position);
    lands.add_executors(ExecutorType::Startup, pos_query);

    lands.run();
    lands.emit(Collision{.collided = true});

    lands.add_task(generate_numbers, 10);

    lands.add_plugin<PluginTest>();
    lands.build_plugins();

    auto f = []() {
        std::println("Hello");
    };
    p(f);

    while (true)
    {

        lands.run_tasks(0.16f);
    }

    return 0;
}
