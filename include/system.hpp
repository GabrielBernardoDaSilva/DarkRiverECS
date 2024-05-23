#ifndef FORGED_IN_LOST_LANDS_ECS_SYSTEM_H
#define FORGED_IN_LOST_LANDS_ECS_SYSTEM_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <print>

#include "executor.hpp"
#include "query.hpp"
#include "forged_concepts.hpp"

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
        Executor(std::function<void(Args...)> func, std::vector<Archetype *> archetypes) : executor(func), archetypes(archetypes)
        {
        }

        Executor(void (*func)(Args...), std::vector<Archetype *> archetypes) : executor(func), archetypes(archetypes)
        {
        }

        void execute() override
        {
            // use forwarding to pass the arguments to the function
            executor(std::forward<Args>(archetypes)...);
        }

    private:
        std::vector<Archetype *> archetypes;
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
        void add_executor(std::function<void(Args...)> func, std::vector<Archetype *> archetypes)
        {
            executors.push_back(std::make_unique<Executor<Args...>>(func, archetypes));
        }

        template <typename... Args>
        void add_executor(void (*func)(Args...), std::vector<Archetype *> archetypes)
        {
            executors.push_back(std::make_unique<Executor<Args...>>(func, archetypes));
        }

        void execute();

    private:
        std::vector<std::unique_ptr<ExecutorBase>> executors;
    };
}

#endif // FORGED_IN_LOST_LANDS_ECS_SYSTEM_H