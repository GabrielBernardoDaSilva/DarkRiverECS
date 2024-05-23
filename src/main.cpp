#include <iostream>
#include <print>
#include <tuple>
#include "lost_lands.hpp"
#include "component.hpp"
#include "archetype.hpp"
#include "query.hpp"
#include "system.hpp"
#include "forged_concepts.hpp"
#include "executor.hpp"

using namespace forged_in_lost_lands_ecs;

struct Velocity : public Component
{
    float x, y;
};

struct Health : public Component
{
    int health;
};

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

    lands.add_executor(&t);
    lands.run();

    // create variable based on args types
    // create_variable(std::declval<f_args_types>());
}
