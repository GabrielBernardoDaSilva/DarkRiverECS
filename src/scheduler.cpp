#include "scheduler.hpp"

namespace forged_in_lost_lands_ecs
{
    TaskScheduler::TaskScheduler(TaskScheduler &&other) noexcept
    {
        task_gen = std::move(other.task_gen);
        is_running = other.is_running;
        amount_of_seconds_to_wait = other.amount_of_seconds_to_wait;
        is_done = other.is_done;
    }

    bool TaskScheduler::execute(float dt)
    {
        if (is_running)
        {

            is_running = true;
            bool task_result = task_gen.value().m_handle.done();
            if (task_result)
            {
                std::println("Task is done");
                is_running = false;
                is_done = true;
                return is_done;
            }
            if (amount_of_seconds_to_wait <= 0.0f)
            {
                std::println("Task is not done");
                amount_of_seconds_to_wait = task_gen.value()().seconds;
            }
            else if (amount_of_seconds_to_wait > 0.0f)
            {
                amount_of_seconds_to_wait -= dt;
            }
        }

        return is_done;
    }

    bool TaskScheduler::task_is_done()
    {
        return is_done;
    }

    void TaskScheduler::set_running(bool running)
    {
        if (!is_done)
            is_running = running;
    }

    TaskManager::TaskManager(TaskManager &&other) noexcept
    {
        tasks = std::move(other.tasks);
    }

    TaskManager &TaskManager::operator=(TaskManager &&other) noexcept
    {
        if (this != &other)
            tasks = std::move(other.tasks);
        return *this;
    }

    TaskId TaskManager::add_task(TaskScheduler &&task)
    {
        tasks.insert({task_id, std::move(task)});
        return TaskId{task_id++};
    }

    void TaskManager::execute_all(float dt)
    {
        for (auto &[id, task] : tasks)
        {
            if (task.execute(dt))
                tasks_to_remove.push_back(id);
        }
    }
    
    std::expected<Success, SchedulerError> TaskManager::stop_task(TaskId id)
    {
        auto task = tasks.find(id.id);
        if (task != tasks.end())
        {
            std::println("Task is stopped");
            task->second.set_running(false);
            return Success{};
        }
        return std::unexpected(SchedulerError::TASK_NOT_FOUND);
    }

    std::expected<Success, SchedulerError> TaskManager::resume_task(TaskId id)
    {
        auto task = tasks.find(id.id);
        if (task != tasks.end())
        {
            task->second.set_running(true);
            return Success{};
        }
        return std::unexpected(SchedulerError::TASK_NOT_FOUND);
    }

    std::expected<Success, SchedulerError> TaskManager::remove_task(TaskId id)
    {
        auto task = tasks.find(id.id);
        if (task != tasks.end())
        {
            tasks.erase(task);
            return Success{};
        }
        return std::unexpected(SchedulerError::TASK_NOT_FOUND);
    }

    void TaskManager::stop_all_tasks()
    {
        for (auto &[_, task] : tasks)
            task.set_running(false);
    }

    void TaskManager::resume_all_tasks()
    {
        for (auto &[_, task] : tasks)
            task.set_running(true);
    }

    void TaskManager::remove_all_tasks_is_done()
    {
        for (std::size_t id : tasks_to_remove)
        {
            std::println("Removing all tasks that are done");

            remove_task(TaskId{id});
        }

        if (!tasks_to_remove.empty())
            tasks_to_remove.clear();
    }
}
