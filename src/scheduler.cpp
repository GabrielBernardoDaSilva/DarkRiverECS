#include "scheduler.hpp"

#include <ranges>

namespace winter_rain_ecs {
    TaskScheduler::TaskScheduler(TaskScheduler &&other) noexcept {
        m_task_gen = std::move(other.m_task_gen);
        m_is_running = other.m_is_running;
        m_amount_of_seconds_to_wait = other.m_amount_of_seconds_to_wait;
        m_is_done = other.m_is_done;
    }

    bool TaskScheduler::execute(const float dt) {
        if (m_is_running) {
            m_is_running = true;
            if (const bool task_result = m_task_gen.value().m_handle.done(); task_result) {
                m_is_running = false;
                m_is_done = true;
                return m_is_done;
            }
            if (m_amount_of_seconds_to_wait <= 0.0f) {
                // transform to seconds
                m_amount_of_seconds_to_wait = m_task_gen.value()().seconds * 1000;
            } else if (m_amount_of_seconds_to_wait > 0.0f) {
                m_amount_of_seconds_to_wait -= dt;
            }
        }

        return m_is_done;
    }

    bool TaskScheduler::task_is_done() const {
        return m_is_done;
    }

    void TaskScheduler::set_running(const bool running) {
        if (!m_is_done)
            m_is_running = running;
    }

    TaskManager::TaskManager(TaskManager &&other) noexcept {
        m_tasks = std::move(other.m_tasks);
    }

    TaskManager &TaskManager::operator=(TaskManager &&other) noexcept {
        if (this != &other)
            m_tasks = std::move(other.m_tasks);
        return *this;
    }

    TaskId TaskManager::add_task(TaskScheduler &&task) {
        m_tasks.insert({m_task_id, std::move(task)});
        return TaskId{m_task_id++};
    }

    void TaskManager::execute_all(const float dt) {
        for (auto &[id, task]: m_tasks) {
            if (task.execute(dt))
                m_tasks_to_remove.push_back(id);
        }
    }

    std::expected<Success, SchedulerError> TaskManager::stop_task(const TaskId id) {
        if (const auto task = m_tasks.find(id.m_id); task != m_tasks.end()) {
            task->second.set_running(false);
            return Success{};
        }
        return std::unexpected(SchedulerError::TaskNotFound);
    }

    std::expected<Success, SchedulerError> TaskManager::resume_task(const TaskId id) {
        if (const auto task = m_tasks.find(id.m_id); task != m_tasks.end()) {
            task->second.set_running(true);
            return Success{};
        }
        return std::unexpected(SchedulerError::TaskNotFound);
    }

    std::expected<Success, SchedulerError> TaskManager::remove_task(const TaskId id) {
        if (const auto task = m_tasks.find(id.m_id); task != m_tasks.end()) {
            m_tasks.erase(task);
            return Success{};
        }
        return std::unexpected(SchedulerError::TaskNotFound);
    }

    void TaskManager::stop_all_tasks() {
        for (auto &task: m_tasks | std::views::values)
            task.set_running(false);
    }

    void TaskManager::resume_all_tasks() {
        for (auto &task: m_tasks | std::views::values)
            task.set_running(true);
    }

    void TaskManager::remove_all_tasks_is_done() {
        for (const std::size_t id: m_tasks_to_remove) {
            auto _result = remove_task(TaskId{id});
        }

        if (!m_tasks_to_remove.empty())
            m_tasks_to_remove.clear();
    }
}
