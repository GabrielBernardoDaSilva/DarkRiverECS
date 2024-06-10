#include "world.hpp"
#include <exception>
#include <iostream>

namespace darkriver
{
    EntityManager &World::get_entity_manager()
    {
        return m_entity_manager;
    }

    std::expected<Success, ArchetypeError> World::remove_entity(const Entity entity)
    {
        return m_entity_manager.remove_entity(entity);
    }

    ExecutorManager &World::get_executor_manager()
    {
        return m_executor_manager;
    }

    void World::run() noexcept
    {
        try
        {
            m_executor_manager.startup_executor();
            m_executor_manager.execute();
            m_executor_manager.shutdown_executor();
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << e.what() << '\n';
            std::exit(1);
        }
    }

    EventManager &World::get_event_manager()
    {
        return m_event_manager;
    }

    void World::build_plugins()
    {
        for (auto &plugin : m_plugins)
        {
            plugin->build(*this);
        }
    }

    void World::run_tasks(const float dt)
    {
        m_task_manager.execute_all(dt);
    }

    std::expected<Success, SchedulerError> World::resume_task(const TaskId id)
    {
        return m_task_manager.resume_task(id);
    }

    std::expected<Success, SchedulerError> World::stop_task(const TaskId id)
    {
        return m_task_manager.stop_task(id);
    }

    std::expected<Success, SchedulerError> World::remove_task(const TaskId id)
    {
        return m_task_manager.remove_task(id);
    }

    void World::stop_all_tasks()
    {
        m_task_manager.stop_all_tasks();
    }

    void World::resume_all_tasks()
    {
        m_task_manager.resume_all_tasks();
    }

    TaskManager &World::get_task_manager()
    {
        return m_task_manager;
    }

    std::vector<Archetype *> World::create_archetype_ref()
    {
        std::vector<Archetype *> archetypes_ptr{};
        archetypes_ptr.reserve(m_entity_manager.m_archetypes.size());
        for (auto &archetype : m_entity_manager.m_archetypes)
        {
            archetypes_ptr.push_back(&archetype);
        }
        return archetypes_ptr;
    }

    void World::show_archetypes() const
    {
        std::println("--------------------Archetypes-------------------");
        for (auto &archetype : m_entity_manager.m_archetypes)
        {
            archetype.list_all_components_hash();
        }
        std::println("------------------------------------------------");
    }

    void World::show_entities() const
    {
        std::println("--------------------Entities-------------------");
        for (auto &entity : m_entity_manager.m_entities)
        {
            std::println("Entity id: {}, Entity Location: {}", entity.id, entity.location);
        }
        std::println("------------------------------------------------");
    }
}
