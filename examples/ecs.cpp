#include <print>
#include <string>
#include "winter_rain_ecs.hpp"

struct Health
{
    int health;
};

struct Position
{
    float x, y;
};

struct Name
{
    std::string name;
};

struct CreepySnakeHair
{
};

struct Attack
{
    int damage;
};

void medusa_attack(winter_rain_ecs::Query<winter_rain_ecs::With<Health &, Position &, Name &, CreepySnakeHair &>> query,
                   winter_rain_ecs::Query<winter_rain_ecs::With<Health &, Position &, Name &>> query2, winter_rain_ecs::EventManager &event_manager)
{

    for (auto [health, pos, name, _] : query.all())
    {
        for (auto [health2, pos2, name2] : query2.all())
        {
            if (name2.name == "Perseus")
            {
                std::println("Medusa attacks Perseus!");
                event_manager.emit(Attack{200});
            }
        }
    }
}

void print_names(winter_rain_ecs::Query<winter_rain_ecs::With<const Name &>> query)
{
    for (auto [name] : query.all())
    {
        std::println("Name: {}", name.name);
    }
}

int main()
{

    winter_rain_ecs::World world;

    world.subscribe<Attack>([](winter_rain_ecs::World &world, const Attack &attack)
                            {
        auto query_perseus = world.query<winter_rain_ecs::With<winter_rain_ecs::Entity&, Health &, Name &>>();
        for (auto [ent, health, name] : query_perseus.all())
        {
            health.health -= attack.damage;
            if (name.name == "Perseus" && health.health < 0)
            {
                std::println("{} was attacked and passed away!", name.name);    
                world.remove_entity(ent);  
            }
        } });

    world.add_entity(Position{0.0f, 0.0f}, Health{100}, Name{"Perseus"});
    world.add_entity(Position{0.0f, 0.0f}, Health{100}, Name{"Medusa"}, CreepySnakeHair{});
    world.add_entity(Position{0.0f, 0.0f}, Health{100}, Name{"Athena"});

    world.add_entity(Position{0.0f, 0.0f}, Health{10000}, Name{"Zeus"});
    world.add_entity(Health{10000}, Name{"Prometheus"});

    world.add_executors(winter_rain_ecs::ExecutorType::Startup, medusa_attack, print_names);

    world.run();

    return 0;
}
