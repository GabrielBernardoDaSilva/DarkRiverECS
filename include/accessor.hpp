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
        explicit Accessor(World &world) : world(world) {}

        std::vector<Archetype *> get_archetypes() const;
        std::size_t get_archetype_size() const;

        EventManager &get_event_manager() const;
        ExecutorManager &get_executor_manager() const;
        TaskManager &get_task_manager() const;
        EntityManager &get_entity_manager() const;
        World &get_world() const;

    private:
        World &world;
    };
}
