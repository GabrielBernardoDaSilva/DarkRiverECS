#pragma once

#include <iostream>
#include <coroutine>
#include <cassert>

namespace forged_in_lost_lands_ecs
{
    template <typename T>
    struct generator
    {
        struct promise_type
        {
            T m_value;
            generator get_return_object() { return generator(this); }
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }

            void unhandled_exception() noexcept
            {
                std::rethrow_exception(std::current_exception());
            }
            std::suspend_always yield_value(T val)
            {
                m_value = val;
                return {};
            }

            void return_void()
            {
                        }
        };
        generator(promise_type *p)
            : m_handle(std::coroutine_handle<promise_type>::from_promise(*p)) {}
        ~generator()
        {
            std::cout << "Handle destroyed..." << std::endl;
            m_handle.destroy();
        }

        T operator()()
        {
            assert(m_handle != nullptr);
            m_handle.resume();
            return (m_handle.promise().m_value);
        }
        std::coroutine_handle<promise_type> m_handle;
    };
}