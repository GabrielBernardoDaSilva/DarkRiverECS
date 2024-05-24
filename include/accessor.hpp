#pragma once
#include <vector>

namespace forged_in_lost_lands_ecs
{
    class LostLands;
    class Archetype;
    class EventManager;
    class ExecutorManager;
    class TaskManager;

    class Accessor
    {
    public:
        Accessor(LostLands &lost_land) : lost_land(lost_land) {}

        std::vector<Archetype *> get_archetypes();
        EventManager &get_event_manager();
        ExecutorManager &get_executor_manager();
        TaskManager &get_task_manager();

    private:
        LostLands &lost_land;
    };
}
