#pragma once
#include <vector>
#include <cstddef>

namespace forged_in_lost_lands_ecs
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
        Accessor(World &lost_land) : lost_land(lost_land) {}

        std::vector<Archetype *> get_archetypes();
        std::size_t get_archetype_size();

        EventManager &get_event_manager();
        ExecutorManager &get_executor_manager();
        TaskManager &get_task_manager();
        EntityManager &get_entity_manager();
        World &get_lost_land();

    private:
        World &lost_land;
    };
}
