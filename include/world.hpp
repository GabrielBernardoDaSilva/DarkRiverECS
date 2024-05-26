#pragma once

#include "archetype.hpp"
#include "system.hpp"
#include "forged_concepts.hpp"
#include "errors.hpp"
#include "system.hpp"
#include "event.hpp"
#include "plugin.hpp"
#include "scheduler.hpp"
#include "generator.hpp"
#include "accessor.hpp"
#include "entity_manager.hpp"

#include <vector>
#include <memory>
#include <cstddef>
#include <ranges>
#include <print>
#include <expected>
#include <functional>
#include <type_traits>
#include <tuple>
#include <string_view>

namespace winter_rain_ecs
{

    class World
    {
    private:
        std::vector<std::unique_ptr<Plugin>> plugins{};
        ExecutorManager executor_manager{};
        EventManager event_manager{};
        TaskManager task_manager{};
        EntityManager entity_manager;

        Accessor accessor{*this};

    public:
        World() = default;
        ~World() = default;

        /// archetypes

        EntityManager &get_entity_manager();

        template <req_component... T>
        Entity add_entity(T... components)
        {
            auto entity = entity_manager.add_entity(components...);
            return entity;
        };

        template <req_component T>
        std::expected<Success, ArchetypeError> add_component_to_entity(Entity entity, T component)
        {
            return entity_manager.add_component_to_entity<T>(entity, component);
        }

        template <req_component T>
        std::expected<Success, ArchetypeError> remove_component_from_entity(Entity entity)
        {
            return entity_manager.remove_component_from_entity<T>(entity);
        }

        std::expected<Success, ArchetypeError> remove_entity(Entity entity);

        /// archetypes end

        /// systems

        template <req_system_args... Args>
        void add_executor(ExecutorType executor_type, std::function<void(Args...)> system)
        {
            executor_manager.add_executor(executor_type, system, accessor);
        }

        template <req_system_args... Args>
        void add_executor(ExecutorType executor_type, void (*system)(Args...))
        {
            executor_manager.add_executor(executor_type, system, accessor);
        }

        template <function_pointer Lambda>
        void add_executor(ExecutorType executor_type, Lambda &&lambda)
        {
            std::function func{lambda};
            executor_manager.add_executor(executor_type, func, accessor);
        }

        template <function_pointer... Executors>
        void add_executors(ExecutorType execute_type, Executors &&...executors)
        {
            (add_executor(execute_type, executors), ...);
        }

        template <validation_query_types... T>
        Query<T...> query()
        {
            return Query<T...>{accessor};
        }

        ExecutorManager &get_executor_manager();

        void run() noexcept;

        ///

        /// events

        template <req_event_ty Ev>
        void emit(Ev event)
        {
            event_manager.emit(event);
        }

        template <req_event_ty Ev>
        void subscribe(std::function<void(Ev)> subscriber)
        {
            event_manager.subscribe<Ev>(subscriber);
        }

        template <req_event_ty Ev>
        void subscribe(void (*subscriber)(Ev))
        {
            event_manager.subscribe<Ev>(subscriber);
        }

        template <req_event_ty Ev>
        void unsubscribe(std::function<void(Ev)> subscriber)
        {
            event_manager.unsubscribe<Ev>(subscriber);
        }

        template <req_event_ty Ev>
        void unsubscribe(void (*subscriber)(Ev))
        {
            event_manager.unsubscribe<Ev>(subscriber);
        }

        EventManager &get_event_manager();

        // plugins
        template <req_plugin T>
        void add_plugin()
        {
            plugins.push_back(std::make_unique<T>());
        }

        void build_plugins();

        //

        // tasks
        template <typename... Args>
        TaskId add_task(std::function<generator<WaitAmountOfSeconds>(Args...)> task, Args... args)
        {
            return task_manager.add_task({task, args...});
        }

        template <typename... Args>
        TaskId add_task(generator<WaitAmountOfSeconds> (*task)(Args...), Args... args)
        {
            return task_manager.add_task({task, args...});
        }

        void run_tasks(float dt);

        void resume_task(TaskId id);

        void stop_task(TaskId id);

        void remove_task(TaskId id);

        void stop_all_tasks();

        void resume_all_tasks();

        TaskManager &get_task_manager();

        std::vector<Archetype *> create_archetype_ref();
        void show_archetypes() const;
    };
}
