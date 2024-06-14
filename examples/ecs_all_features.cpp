#include <print>
#include <functional>
#include <thread>
#include <chrono>

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

    Health(int health) : health(health)
    {
        std::println("Constructor");
    }

    Health(const Health &other) = delete;
    Health(Health &&other) noexcept : health(std::move(other.health))
    {
        std::println("Move Constructor");
    }

    Health &operator=(const Health &other) = delete;

    Health &operator=(Health &&other) noexcept
    {
        health = std::move(other.health);
        std::println("Move Assignment");
        return *this;
    }
};

generator<WaitAmountOfMilleSeconds> generate_numbers(World *world, int i)
{
    std::println("generate_numbers starting");
    co_yield WaitAmountOfMilleSeconds{
        10.0f};
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
        100};

    World world;
    world.add_plugin<PluginTest>();
    world.build_plugins();
    Entity e = world.add_entity(pos, vel);
    Entity e2 = world.add_entity(pos2, vel2);
    Entity e3 = world.add_entity(pos3, std::move(health));

    world.add_component_to_entity(e, Health{300});
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

    // in case you need the world pass as pointer to has cohesion
    world.add_task(generate_numbers, &world, 10);

    auto f = []()
    {
        std::println("Hello");
    };
    p(f);

    std::println("Run");
    int i = 0;
    while (true)
    {
        world.run_tasks(1.0f);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::println("{}", i++);
    }
    return 0;
}