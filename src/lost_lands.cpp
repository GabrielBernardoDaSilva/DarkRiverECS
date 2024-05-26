#include "world.hpp"
#include <exception>
#include <iostream>

namespace winter_rain_ecs
{
    EntityManager &World::get_entity_manager()
    {
        return entity_manager;
    }

    std::expected<Success, ArchetypeError> World::remove_entity(Entity entity)
    {
        return entity_manager.remove_entity(entity);
    }

    ExecutorManager &World::get_executor_manager()
    {
        return executor_manager;
    }

    void World::run() noexcept
    {
        try
        {
            executor_manager.startup_executor();
            executor_manager.execute();
            executor_manager.shutdown_executor();
        }
        catch (const std::invalid_argument e)
        {
            std::cerr << e.what() << '\n';
            std::exit(1);
        }
    }
    EventManager &World::get_event_manager()
    {
        return event_manager;
    }
    void World::build_plugins()
    {
        for (auto &plugin : plugins)
        {
            plugin->build(*this);
        }
    }
    void World::run_tasks(float dt)
    {
        task_manager.execute_all(dt);
    }
    void World::resume_task(TaskId id)
    {
        task_manager.resume_task(id);
    }
    void World::stop_task(TaskId id)
    {
        task_manager.stop_task(id);
    }
    void World::remove_task(TaskId id)
    {
        task_manager.remove_task(id);
    }
    void World::stop_all_tasks()
    {
        task_manager.stop_all_tasks();
    }
    void World::resume_all_tasks()
    {
        task_manager.resume_all_tasks();
    }
    TaskManager &World::get_task_manager()
    {
        return task_manager;
    }
    std::vector<Archetype *> World::create_archetype_ref()
    {
        std::vector<Archetype *> archetypes_ptr{};
        for (auto &archetype : entity_manager.archetypes)
        {
            archetypes_ptr.push_back(&archetype);
        }
        return archetypes_ptr;
    }

    void World::show_archetypes() const
    {
        std::println("--------------------Archetypes-------------------");
        for (auto &archetype : entity_manager.archetypes)
        {
            archetype.list_all_components_hash();
        }
        std::println("------------------------------------------------");
    }
}
