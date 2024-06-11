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

generator<WaitAmountOfSeconds> generate_numbers(int i)
{
    std::println("generate_numbers starting");
    co_yield WaitAmountOfSeconds{
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
   
    return 0;
}