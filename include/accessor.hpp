#pragma once
#include <vector>
#include <cstddef>

namespace winter_rain_ecs
{
    class World;

    class Archetype;
    class EventManager;
    class ExecutorManager;
    class TaskManager;
    class EntityManager;

    class Accessor
    {
    public:
        Accessor(World &world) : world(world) {}

        std::vector<Archetype *> get_archetypes();
        std::size_t get_archetype_size();

        EventManager &get_event_manager();
        ExecutorManager &get_executor_manager();
        TaskManager &get_task_manager();
        EntityManager &get_entity_manager();
        World &get_world();

    private:
        World &world;
    };
}
