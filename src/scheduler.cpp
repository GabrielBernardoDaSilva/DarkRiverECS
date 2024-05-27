#include "scheduler.hpp"

#include <ranges>

namespace winter_rain_ecs {
    TaskScheduler::TaskScheduler(TaskScheduler &&other) noexcept {
        task_gen = std::move(other.task_gen);
        is_running = other.is_running;
        amount_of_seconds_to_wait = other.amount_of_seconds_to_wait;
        is_done = other.is_done;
    }

    bool TaskScheduler::execute(const float dt) {
        if (is_running) {
            is_running = true;
            if (const bool task_result = task_gen.value().m_handle.done(); task_result) {
                is_running = false;
                is_done = true;
                return is_done;
            }
            if (amount_of_seconds_to_wait <= 0.0f) {
                // transform to seconds
                amount_of_seconds_to_wait = task_gen.value()().seconds * 1000;
            } else if (amount_of_seconds_to_wait > 0.0f) {
                amount_of_seconds_to_wait -= dt;
            }
        }

        return is_done;
    }

    bool TaskScheduler::task_is_done() const {
        return is_done;
    }

    void TaskScheduler::set_running(const bool running) {
        if (!is_done)
            is_running = running;
    }

    TaskManager::TaskManager(TaskManager &&other) noexcept {
        tasks = std::move(other.tasks);
    }

    TaskManager &TaskManager::operator=(TaskManager &&other) noexcept {
        if (this != &other)
            tasks = std::move(other.tasks);
        return *this;
    }

    TaskId TaskManager::add_task(TaskScheduler &&task) {
        tasks.insert({task_id, std::move(task)});
        return TaskId{task_id++};
    }

    void TaskManager::execute_all(const float dt) {
        for (auto &[id, task]: tasks) {
            if (task.execute(dt))
                tasks_to_remove.push_back(id);
        }
    }

    std::expected<Success, SchedulerError> TaskManager::stop_task(const TaskId id) {
        if (const auto task = tasks.find(id.id); task != tasks.end()) {
            task->second.set_running(false);
            return Success{};
        }
        return std::unexpected(SchedulerError::TaskNotFound);
    }

    std::expected<Success, SchedulerError> TaskManager::resume_task(const TaskId id) {
        if (const auto task = tasks.find(id.id); task != tasks.end()) {
            task->second.set_running(true);
            return Success{};
        }
        return std::unexpected(SchedulerError::TaskNotFound);
    }

    std::expected<Success, SchedulerError> TaskManager::remove_task(const TaskId id) {
        if (const auto task = tasks.find(id.id); task != tasks.end()) {
            tasks.erase(task);
            return Success{};
        }
        return std::unexpected(SchedulerError::TaskNotFound);
    }

    void TaskManager::stop_all_tasks() {
        for (auto &task: tasks | std::views::values)
            task.set_running(false);
    }

    void TaskManager::resume_all_tasks() {
        for (auto &task: tasks | std::views::values)
            task.set_running(true);
    }

    void TaskManager::remove_all_tasks_is_done() {
        for (const std::size_t id: tasks_to_remove) {
            auto resutl = remove_task(TaskId{id});
        }

        if (!tasks_to_remove.empty())
            tasks_to_remove.clear();
    }
}
