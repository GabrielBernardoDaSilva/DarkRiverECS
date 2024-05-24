#pragma once

#include <coroutine>
#include <functional>
#include <print>
#include <cstddef>
#include <algorithm>
#include <ranges>
#include <expected>

#include "generator.hpp"
#include "errors.hpp"

namespace forged_in_lost_lands_ecs
{
    struct WaitAmountOfSeconds
    {
        float seconds;
    };

    class TaskScheduler
    {
    public:
        template <typename... Args>
        TaskScheduler(std::function<generator<WaitAmountOfSeconds>(Args...)> task, Args... args) : task_gen(task(args...))
        {
            is_running = true;
        }

        template <typename... Args>
        TaskScheduler(generator<WaitAmountOfSeconds> (*task)(Args...), Args... args) : task_gen(task(args...))
        {
            is_running = true;
        }

        void execute(float dt)
        {
            if (is_running)
            {

                is_running = true;
                bool task_result = task_gen.m_handle.done();
                if (task_result)
                {
                    std::println("Task is done");
                    is_running = false;
                    is_done = true;
                    return;
                }
                if (amount_of_seconds_to_wait <= 0.0f){
                    std::println("Task is not done");
                    amount_of_seconds_to_wait = task_gen().seconds;
                }
                else if (amount_of_seconds_to_wait > 0.0f)
                {
                    amount_of_seconds_to_wait -= dt;
                }
            }
        }

        bool task_is_done()
        {
            return is_done;
        }

        void set_running(bool running)
        {
            if (!is_done)
                is_running = running;
        }

    private:
        bool is_running = false;
        float amount_of_seconds_to_wait = 0.0f;
        bool is_done = false;
        generator<WaitAmountOfSeconds> task_gen;
    };

    class TaskId
    {
        friend class TaskManager;

    public:
        TaskId(std::size_t id) : id{id} {}

    private:
        std::size_t id{0};
    };

    class TaskManager
    {

    public:
        TaskManager() = default;
        ~TaskManager() = default;

        TaskManager(const TaskManager &) = delete;
        TaskManager &operator=(const TaskManager &) = delete;

        // move constructor
        TaskManager(TaskManager &&other) noexcept
        {
            tasks = std::move(other.tasks);
        }

        // move assignment
        TaskManager &operator=(TaskManager &&other) noexcept
        {
            if (this != &other)
                tasks = std::move(other.tasks);
            return *this;
        }

        [[nodiscard]] TaskId add_task(TaskScheduler task)
        {
            tasks.push_back(task);
            return TaskId{task_id++};
        }

        void execute_all(float dt)
        {
            for (auto &task : tasks)
                task.execute(dt);
        }

        std::expected<Success, SchedulerError> stop_task(TaskId id)
        {
            auto task = std::ranges::find_if(tasks, [&](TaskScheduler &task)
                                             { return task_id == id.id; });
            if (task != tasks.end())
            {
                task->set_running(false);
                return Success{};
            }
            return std::unexpected(SchedulerError::TASK_NOT_FOUND);
        }

        std::expected<Success, SchedulerError> resume_task(TaskId id)
        {
            auto task = std::ranges::find_if(tasks, [&](TaskScheduler &task)
                                             { return task_id == id.id; });
            if (task != tasks.end())
            {
                task->set_running(true);
                return Success{};
            }
            return std::unexpected(SchedulerError::TASK_NOT_FOUND);
        }

        std::expected<Success, SchedulerError> remove_task(TaskId id)
        {
            auto task = std::ranges::find_if(tasks, [&](TaskScheduler &task)
                                             { return task_id == id.id; });
            if (task != tasks.end())
            {
                tasks.erase(task);
                return Success{};
            }
            return std::unexpected(SchedulerError::TASK_NOT_FOUND);
        }

        void stop_all_tasks()
        {
            std::ranges::for_each(tasks, [](TaskScheduler &task)
                                  { task.set_running(false); });
        }

        void resume_all_tasks()
        {
            std::ranges::for_each(tasks, [](TaskScheduler &task)
                                  { task.set_running(true); });
        }

        void remove_all_tasks_is_done()
        {
            bool is_done = false;
            while (!is_done)
            {

                auto res = std::ranges::remove_if(tasks, [](TaskScheduler &task)
                                                  { return task.task_is_done(); });
                if (res.begin() != tasks.end())
                    tasks.erase(res.begin(), res.end());
                else
                    is_done = true;
            }
        }

    private:
        std::vector<TaskScheduler>
            tasks{};
        std::size_t task_id{0};
    };
}