#include "accessor.hpp"

#include "world.hpp"

namespace forged_in_lost_lands_ecs
{
    std::vector<Archetype *> Accessor::get_archetypes()
    {
        return world.create_archetype_ref();
    }

    std::size_t Accessor::get_archetype_size()
    {
        return world.get_entity_manager().get_archetype_size();
    }

    EventManager &Accessor::get_event_manager()
    {
        return world.get_event_manager();
    }

    ExecutorManager &Accessor::get_executor_manager()
    {
        return world.get_executor_manager();
    }

    TaskManager &Accessor::get_task_manager()
    {
        return world.get_task_manager();
    }

    EntityManager &Accessor::get_entity_manager()
    {
        return world.get_entity_manager();
    }
    World &Accessor::get_world()
    {
        return world;
    }
}