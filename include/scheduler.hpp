#pragma once

#include <coroutine>
#include <functional>
#include <print>

#include "generator.hpp"

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
                auto t = task_gen.m_handle.done();
                if (t)
                {
                    is_running = false;
                    return;
                }
                if (amount_of_seconds_to_wait <= 0.0f)
                    amount_of_seconds_to_wait = task_gen().seconds;
                else if (amount_of_seconds_to_wait > 0.0f)
                {
                    amount_of_seconds_to_wait -= dt;
                }
            }
        }

    private:
        bool is_running = false;
        float amount_of_seconds_to_wait = 0.0f;
        generator<WaitAmountOfSeconds> task_gen;
    };
}