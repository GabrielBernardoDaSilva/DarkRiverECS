#include "accessor.hpp"

#include "lost_lands.hpp"

namespace forged_in_lost_lands_ecs
{
    std::vector<Archetype *> Accessor::get_archetypes()
    {
        return lost_land.create_archetype_ref();
    }

    std::size_t Accessor::get_archetype_size()
    {
        return lost_land.get_entity_manager().get_archetype_size();
    }

    EventManager &Accessor::get_event_manager()
    {
        return lost_land.get_event_manager();
    }

    ExecutorManager &Accessor::get_executor_manager()
    {
        return lost_land.get_executor_manager();
    }

    TaskManager &Accessor::get_task_manager()
    {
        return lost_land.get_task_manager();
    }

    EntityManager &Accessor::get_entity_manager()
    {
        return lost_land.get_entity_manager();
    }
    LostLands &Accessor::get_lost_land()
    {
        return lost_land;
    }
}