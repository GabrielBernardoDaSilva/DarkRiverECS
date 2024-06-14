#pragma once

#include "generator.hpp"
#include "errors.hpp"
#include <functional>
#include <cstddef>
#include <expected>
#include <optional>
#include <map>
#include <vector>

namespace darkriver
{
    struct WaitAmountOfSeconds
    {
        float m_mille_seconds;
    };

    class TaskScheduler
    {
    public:
        template <typename... Args>
        explicit TaskScheduler(std::function<generator<WaitAmountOfSeconds>(Args...)> task, Args... args) : m_task_gen(
                                                                                                                task(args...))
        {
            m_is_running = true;
        }

        template <typename... Args>
        explicit TaskScheduler(generator<WaitAmountOfSeconds> (*task)(Args...),
                               Args... args) : m_task_gen(task(args...))
        {
            m_is_running = true;
        }

        // move constructor
        TaskScheduler(TaskScheduler &&other) noexcept;

        bool execute(float dt);

        [[nodiscard]] bool task_is_done() const;

        void set_running(bool running);

    private:
        bool m_is_running = false;
        float m_amount_of_seconds_to_wait = 0.0f;
        bool m_is_done = false;
        std::optional<generator<WaitAmountOfSeconds>> m_task_gen;
    };

    class TaskId
    {
        friend class TaskManager;

    public:
        explicit TaskId(std::size_t id) : m_id{id}
        {
        }

    private:
        std::size_t m_id{0};
    };

    class TaskManager
    {
    public:
        TaskManager() = default;

        ~TaskManager() = default;

        TaskManager(const TaskManager &) = delete;

        TaskManager &operator=(const TaskManager &) = delete;

        // move constructor
        TaskManager(TaskManager &&other) noexcept;

        // move assignment
        TaskManager &operator=(TaskManager &&other) noexcept;

        TaskId add_task(TaskScheduler &&task);

        void execute_all(float dt);

        std::expected<Success, SchedulerError> stop_task(TaskId id);

        std::expected<Success, SchedulerError> resume_task(TaskId id);

        std::expected<Success, SchedulerError> remove_task(TaskId id);

        void stop_all_tasks();

        void resume_all_tasks();

        void remove_all_tasks_is_done();

    private:
        std::map<std::size_t, TaskScheduler> m_tasks{};
        std::size_t m_task_id{0};
        std::vector<std::size_t> m_tasks_to_remove{};
    };
}
