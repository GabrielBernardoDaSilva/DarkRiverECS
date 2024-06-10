#include <print>
#include <string>
#include "darkriver.hpp"

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

void medusa_attack(darkriver::Query<darkriver::With<Health &, Position &, Name &, CreepySnakeHair &>> query,
                   darkriver::Query<darkriver::With<Health &, Position &, Name &>> query2, darkriver::EventManager &event_manager)
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

void print_names(darkriver::Query<darkriver::With<const Name &>> query)
{
    for (auto [name] : query.all())
    {
        std::println("Name: {}", name.name);
    }
}

int main()
{

    darkriver::World world;

    world.subscribe<Attack>([](darkriver::World &world, const Attack &attack)
                            {
        auto query_perseus = world.query<darkriver::With<darkriver::Entity&, Health &, Name &>>();
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

    world.add_executors(darkriver::ExecutorType::Startup, medusa_attack, print_names);

    world.run();

    return 0;
}
