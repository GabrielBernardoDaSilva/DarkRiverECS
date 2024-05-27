#pragma once

#include "generator.hpp"
#include "errors.hpp"
#include <functional>
#include <cstddef>
#include <expected>
#include <optional>
#include <map>
#include <vector>

namespace winter_rain_ecs {
    struct WaitAmountOfSeconds {
        float seconds;
    };

    class TaskScheduler {
    public:
        template<typename... Args>
        explicit TaskScheduler(std::function<generator<WaitAmountOfSeconds>(Args...)> task, Args... args) : task_gen(
            task(args...)) {
            is_running = true;
        }

        template<typename... Args>
        explicit TaskScheduler(generator<WaitAmountOfSeconds> (*task)(Args...),
                               Args... args) : task_gen(task(args...)) {
            is_running = true;
        }

        // move constructor
        TaskScheduler(TaskScheduler &&other) noexcept;

        bool execute(float dt);

        [[nodiscard]] bool task_is_done() const;

        void set_running(bool running);

    private:
        bool is_running = false;
        float amount_of_seconds_to_wait = 0.0f;
        bool is_done = false;
        std::optional<generator<WaitAmountOfSeconds> > task_gen;
    };

    class TaskId {
        friend class TaskManager;

    public:
        explicit TaskId(std::size_t id) : id{id} {
        }

    private:
        std::size_t id{0};
    };

    class TaskManager {
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
        std::map<std::size_t, TaskScheduler> tasks{};
        std::size_t task_id{0};
        std::vector<std::size_t> tasks_to_remove{};
    };
}
