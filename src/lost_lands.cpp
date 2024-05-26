#include "lost_lands.hpp"
#include <exception>
#include <iostream>

namespace forged_in_lost_lands_ecs
{
    EntityManager &LostLands::get_entity_manager()
    {
        return entity_manager;
    }

    std::expected<Success, ArchetypeError> LostLands::remove_entity(Entity entity)
    {
        return entity_manager.remove_entity(entity);
    }

    ExecutorManager &LostLands::get_executor_manager()
    {
        return executor_manager;
    }

    void LostLands::run() noexcept
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
    EventManager &LostLands::get_event_manager()
    {
        return event_manager;
    }
    void LostLands::build_plugins()
    {
        for (auto &plugin : plugins)
        {
            plugin->build(*this);
        }
    }
    void LostLands::run_tasks(float dt)
    {
        task_manager.execute_all(dt);
    }
    void LostLands::resume_task(TaskId id)
    {
        task_manager.resume_task(id);
    }
    void LostLands::stop_task(TaskId id)
    {
        task_manager.stop_task(id);
    }
    void LostLands::remove_task(TaskId id)
    {
        task_manager.remove_task(id);
    }
    void LostLands::stop_all_tasks()
    {
        task_manager.stop_all_tasks();
    }
    void LostLands::resume_all_tasks()
    {
        task_manager.resume_all_tasks();
    }
    TaskManager &LostLands::get_task_manager()
    {
        return task_manager;
    }
    std::vector<Archetype *> LostLands::create_archetype_ref()
    {
        std::vector<Archetype *> archetypes_ptr{};
        for (auto &archetype : entity_manager.archetypes)
        {
            archetypes_ptr.push_back(&archetype);
        }
        return archetypes_ptr;
    }

    void LostLands::show_archetypes() const
    {
        std::println("--------------------Archetypes-------------------");
        for (auto &archetype : entity_manager.archetypes)
        {
            archetype.list_all_components_hash();
        }
        std::println("------------------------------------------------");
    }
}
