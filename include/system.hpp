#pragma once

#include "executor.hpp"
#include "query.hpp"
#include "forged_concepts.hpp"
#include "accessor.hpp"

#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <print>
#include <type_traits>

namespace winter_rain_ecs
{
    class QueryBase;
    class EntityManager;
    class EventManager;
    class TaskManager;
    class ExecutorManager;

    class ExecutorBase
    {
    public:
        ExecutorBase() = default;
        virtual ~ExecutorBase() = default;

        virtual void execute() = 0;
    };

    template <req_system_args... Args>
    class Executor : public ExecutorBase
    {
    public:
        Executor(std::function<void(Args...)> func, Accessor &accessor) : m_executor(func), m_accessor(accessor)
        {
        }

        Executor(void (*func)(Args...), Accessor &accessor) : m_executor(func), m_accessor(accessor)
        {
        }

        template <typename Arg>
        void print_argument()
        {
            bool is_base_query = std::is_base_of<QueryBase, Arg>::value;
            bool is_event_manager = std::is_same_v<EventManager, std::remove_reference_t<Arg>>;
            bool is_task_manager = std::is_same_v<TaskManager, std::remove_reference_t<Arg>>;
            bool is_executor_manager = std::is_same_v<ExecutorManager, std::remove_reference_t<Arg>>;
            bool is_entity_manager = std::is_same_v<EntityManager, std::remove_reference_t<Arg>>;
            std::println("is_base_query: {}", is_base_query);
            std::println("is_event_manager: {}", is_event_manager);
            std::println("is_task_manager: {}", is_task_manager);
            std::println("is_executor_manager: {}", is_executor_manager);
            std::println("is_entity_manager: {}", is_entity_manager);

            std::println("type: {}", typeid(Arg).name());
        }

        template <typename Arg>
        Arg get_argument()
        {
            constexpr bool is_base_query = std::is_base_of<QueryBase, Arg>::value;
            constexpr bool is_event_manager = std::is_same_v<EventManager, std::remove_reference_t<Arg>>;
            constexpr bool is_task_manager = std::is_same_v<TaskManager, std::remove_reference_t<Arg>>;
            constexpr bool is_executor_manager = std::is_same_v<ExecutorManager, std::remove_reference_t<Arg>>;
            constexpr bool is_entity_manager = std::is_same_v<EntityManager, std::remove_reference_t<Arg>>;
            if constexpr (is_base_query)
                return Arg{m_accessor};
            else if constexpr (is_event_manager)
                return static_cast<Arg>(m_accessor.get_event_manager());
            else if constexpr (is_task_manager)
                return static_cast<Arg>(m_accessor.get_task_manager());
            else if constexpr (is_executor_manager)
                return static_cast<Arg>(m_accessor.get_executor_manager());
            else if constexpr (is_entity_manager)
                return static_cast<Arg>(m_accessor.get_entity_manager());
            throw std::invalid_argument("Invalid argument type");
        }

        void execute() override
        {
            try
            {
                m_executor(get_argument<Args>()...);
            }
            catch (const std::invalid_argument& e)
            {
                throw e;
            }
        }

    private:
        Accessor &m_accessor;
        std::function<void(Args...)> m_executor;
    };

    class ExecutorManager
    {
    public:
        ExecutorManager() = default;
        ExecutorManager(const ExecutorManager &) = default;
        ExecutorManager(ExecutorManager &&) noexcept = default;
        ExecutorManager &operator=(const ExecutorManager &) = default;

        ~ExecutorManager() = default;

        template <typename... Args>
        void add_executor(const ExecutorType executor_type, std::function<void(Args...)> func, Accessor &accessor)
        {
            m_executors[executor_type].push_back(std::make_unique<Executor<Args...>>(func, accessor));
        }

        template <typename... Args>
        void add_executor(const ExecutorType executor_type, void (*func)(Args...), Accessor &accessor)
        {
            m_executors[executor_type].push_back(std::make_unique<Executor<Args...>>(func, accessor));
        }

        void startup_executor();

        void execute();

        void shutdown_executor();

    private:
        std::unordered_map<ExecutorType, std::vector<std::unique_ptr<ExecutorBase>>> m_executors{};
    };
}
