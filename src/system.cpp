#include "system.hpp"
#include "world.hpp"

#include <iostream>
#include <exception>
#include <algorithm>
#include <ranges>

namespace darkriver
{
    void ExecutorManager::startup_executor()
    {
        try
        {
            for (const auto &executor : m_executors[ExecutorType::Startup])
            {
                executor->execute();
            }
        }
        catch (const std::invalid_argument &e)
        {
            throw e;
        }
    }

    void ExecutorManager::execute()
    {
        try
        {
            for (const auto &executor : m_executors[ExecutorType::Update])
            {
                executor->execute();
            }

            // remove once executors
            auto result = std::ranges::remove_if(m_executors[ExecutorType::Update], [](const auto &executor) {
                return executor->get_behaviour() == ExecutorBehaviour::Once;
            });

            m_executors[ExecutorType::Update].erase(result.begin(), result.end());
        }
        catch (const std::invalid_argument &e)
        {
            throw e;
        }
    }
    void ExecutorManager::shutdown_executor()
    {
        try
        {
            for (const auto &executor : m_executors[ExecutorType::End])
            {
                executor->execute();
            }
        }
        catch (const std::invalid_argument &e)
        {
            throw e;
        }
    }
}