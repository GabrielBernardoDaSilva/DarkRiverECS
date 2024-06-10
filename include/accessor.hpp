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
        explicit Accessor(World &world) : m_world(world) {}

        [[nodiscard]] std::vector<Archetype *> get_archetypes() const;
        [[nodiscard]] std::size_t get_archetype_size() const;

        [[nodiscard]] EventManager &get_event_manager() const;
        [[nodiscard]] ExecutorManager &get_executor_manager() const;
        [[nodiscard]] TaskManager &get_task_manager() const;
        [[nodiscard]] EntityManager &get_entity_manager() const;
        [[nodiscard]] World &get_world() const;

    private:
        World &m_world;
    };
}
