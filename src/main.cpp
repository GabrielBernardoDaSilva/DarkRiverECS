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

void t(Query<Position &> query, Query<Velocity &> query1)
{
    auto a = query.all();
    auto b = query1.all();
    for (auto &[pos] : a)
    {
        std::println("Position: x: {} y: {}", pos.x, pos.y);
        pos.x += 1.0f;
    }

    for (auto &[vel] : b)
    {
        std::println("Velocity: x: {} y: {}", vel.x, vel.y);
        vel.x += 1.0f;
    }
}

struct Collision : public Event
{
    bool collided;
};

void check_collision(Collision collision)
{
    std::println("Collision: {}", collision.collided);
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

    // Archetype archetype(entity, pos, vel);
    // archetype.add_entity(entity2, pos2, vel2);

    // Archetype archetype2(entity3, pos3);

    // auto has_pos = archetype.has_component<Position>();

    ////auto res = archetype.remove_component<Velocity>(entity);
    ////if (res.has_value()) {
    ////	std::println("Removed Position");
    ////	archetype2.migrate_entity_to_itself(std::move(res.value()));
    ////}
    ////archetype.add_components<Position, Velocity>(pos, vel);
    // std::vector<Archetype*> archetypes = { &archetype, &archetype2 };
    //{
    //	Query<Position&> query{ archetypes };
    //	auto a = query.all();
    //	for (auto& [pos] : a) {
    //		std::println("Position: x: {} y: {}", pos.x, pos.y);
    //		pos.x += 1.0f;

    //	}
    //}
    // archetype.list_all_components_hash();
    //{
    //	Query<Position&, Velocity&> query{ archetypes };
    //	auto a = query.all();
    //	for (auto& [pos, vel] : a) {
    //		std::println("Position: x: {} y: {}", pos.x, pos.y);
    //		std::println("Velocity: x: {} y: {}", vel.x, vel.y);
    //		pos.x += 1.0f;
    //	}
    //}

    LostLands lands;
    Entity e = lands.add_entity(pos, vel);
    Entity e2 = lands.add_entity(pos2, vel2);

    lands.add_component_to_entity(e, Health{.health = 100});
    lands.show_archetypes();
    lands.remove_component_from_entity<Health>(e);
    lands.show_archetypes();

    // auto query = lands.query<Entity &, Position &>();

    // auto& s1 = lands.create_system();
    // s1.add_query<Entity &, Position &>();
    // auto q1 = s1.get_query<Entity &, Position &>();
    // system.set_executor(ExecutorType::SETUP);

    using f_types = function_traits<decltype(&t)>;
    using f_return_types = f_types::return_type;
    using f_args_types = f_types::argument_types;
    print_tuple_types<f_args_types>();

    // Executor manager{
    //     &t, lands.create_archetype_ref()};
    // manager.execute();

    lands.add_executor(t);
    // lands.emit(Collision{.collided = true});

    lands.subscribe<Collision>(check_collision);
    lands.emit(Collision{.collided = true});
    lands.unsubscribe<Collision>(check_collision);

    lands.run();

    // create variable based on args types
    // create_variable(std::declval<f_args_types>());

    // std::cout << "Function name: " << (FUNCTION_TRAITS(check_collision)) << std::endl;

    // TaskScheduler scheduler{generate_numbers, 10};

    // TaskManager task_manager;
    // auto id = task_manager.add_task({generate_numbers, 10});

    lands.add_task(generate_numbers, 10);

    lands.add_plugin<PluginTest>();
    lands.build_plugins();

    // task_manager.stop_task(id);
    // task_manager.resume_task(id);
    // task_manager.remove_task(id);

    while (true)
    {

        lands.run_tasks(0.16f);
        // lands.run_tasks(0.16f)
        // task_manager.execute_all(0.16f);
        // task_manager.remove_all_tasks_is_done();
        // scheduler.execute(0.16f);
        /* code */
    }

    return 0;
}
