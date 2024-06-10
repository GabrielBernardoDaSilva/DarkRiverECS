#include "accessor.hpp"

#include "world.hpp"

namespace winter_rain_ecs
{
    std::vector<Archetype *> Accessor::get_archetypes() const {
        return m_world.create_archetype_ref();
    }

    std::size_t Accessor::get_archetype_size() const {
        return m_world.get_entity_manager().get_archetype_size();
    }

    EventManager &Accessor::get_event_manager() const {
        return m_world.get_event_manager();
    }

    ExecutorManager &Accessor::get_executor_manager() const {
        return m_world.get_executor_manager();
    }

    TaskManager &Accessor::get_task_manager() const {
        return m_world.get_task_manager();
    }

    EntityManager &Accessor::get_entity_manager() const {
        return m_world.get_entity_manager();
    }

    World &Accessor::get_world() const {
        return m_world;
    }
}