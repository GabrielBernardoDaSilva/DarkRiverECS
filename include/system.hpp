#pragma once

#include "executor.hpp"
#include "query.hpp"
#include "forged_concepts.hpp"
#include "accessor.hpp"
#include "event.hpp"

#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <print>
#include <type_traits>

namespace forged_in_lost_lands_ecs
{

    class ExecutorBase
    {
    public:
        ExecutorBase() = default;
        virtual ~ExecutorBase() = default;

        virtual void execute() = 0;
    };

    template <typename... Args>
    class Executor : public ExecutorBase
    {
    public:
        Executor(std::function<void(Args...)> func, Accessor &accessor) : executor(func), accessor(accessor)
        {
        }

        Executor(void (*func)(Args...), Accessor &accessor) : executor(func), accessor(accessor)
        {
        }

        template <typename Arg>
        void print_argument()
        {
            bool is_base_query = std::is_base_of<QueryBase, Arg>::value;
            bool is_event_manager = std::is_same_v<EventManager, std::remove_reference_t<Arg>>;
            std::println("is_base_query: {}", is_base_query);
            std::println("is_event_manager: {}", is_event_manager);
            std::println("type: {}", typeid(Arg).name());
        }

        template <typename Arg>
        Arg get_argument()
        {
            constexpr bool is_base_query = std::is_base_of<QueryBase, Arg>::value;
            constexpr bool is_event_manager = std::is_same_v<EventManager, std::remove_reference_t<Arg>>;
            if constexpr(is_base_query)
                return Arg{accessor.get_archetypes()};
            else if constexpr(is_event_manager)
                return static_cast<Arg>(accessor.get_event_manager());
            else
            {
                throw std::invalid_argument("Invalid argument type");
            }
        }

        void execute() override
        {
            (print_argument<Args>(), ...);
            // use forwarding to pass the arguments to the function
            // auto args = std::make_tuple(get_argument<Args>()...);
            // executor(std::forward<Args>(accessor.get_archetypes())...);
            // auto args = std::make_tuple(get_argument<Args>()...);
            executor(get_argument<Args>()...);
        }

    private:
        Accessor &accessor;
        std::function<void(Args...)> executor;
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
        void add_executor(std::function<void(Args...)> func, Accessor &accessor)
        {
            executors.push_back(std::make_unique<Executor<Args...>>(func, accessor));
        }

        template <typename... Args>
        void add_executor(void (*func)(Args...), Accessor &accessor)
        {
            executors.push_back(std::make_unique<Executor<Args...>>(func, accessor));
        }

        void execute();

    private:
        std::vector<std::unique_ptr<ExecutorBase>> executors;
    };
}
