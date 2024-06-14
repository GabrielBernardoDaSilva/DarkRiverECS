#include <print>
#include <functional>
#include <string>
#include <chrono>

#include "darkriver.hpp"

using namespace darkriver;
struct Position
{
    float x, y;
};

struct Name
{
    std::string name;
};

void init_all_pawns(EntityManager &entity_manager)
{
    for (std::size_t i = 0; i < 1000000; i++)
    {
        entity_manager
            .add_entity(Position{.x = static_cast<float>(i), .y = static_cast<float>(i)}, Name{.name = std::string("Pawn " + std::to_string(i))});
    }

    std::println("All pawns initialized");
}

void move_all_pawns(Query<With<Position &>> query)
{
    auto a = query.all();
    for (auto &[pos] : a)
    {
        pos.x += 1.0f;
        pos.y += 1.0f;
    }
}

void print_all_pawns(Query<With<const Position &, const Name &>> query)
{
    auto a = query.all();
    for (auto &[pos, name] : a)
    {
        std::println("Name: {}", name.name);
        std::println("Position: x: {} y: {}", pos.x, pos.y);
    }
}

int main()
{
    // start timer 
    auto start = std::chrono::high_resolution_clock::now();
    World world;
    world.add_executor(ExecutorType::Startup, init_all_pawns);
    world.add_executor(ExecutorType::Startup, move_all_pawns);
    world.add_executor(ExecutorType::Startup, print_all_pawns);

    world.run();

    // end timer
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::println("Elapsed time: {}", elapsed.count());
    // 2 minutes and 24 seconds Debug
    // 2 minutes and 05 seconds Release
    return 0;
}