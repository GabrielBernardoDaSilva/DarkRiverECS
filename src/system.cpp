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
                if (!executor->is_executed())
                    executor->execute();

                if (executor->get_behaviour() == ExecutorBehaviour::Once)
                    executor->set_executed(true);
            }
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