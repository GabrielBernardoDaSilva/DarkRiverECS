#include "system.hpp"
#include "lost_lands.hpp"

#include <iostream>
#include <exception>

namespace forged_in_lost_lands_ecs
{
    void ExecutorManager::startup_executor()
    {
        try
        {
            for (auto &executor : executors[ExecutorType::Startup])
            {
                executor->execute();
            }
        }
        catch (const std::invalid_argument e)
        {
            throw e;
        }
    }

    void ExecutorManager::execute()
    {
        try
        {
            for (auto &executor : executors[ExecutorType::Update])
            {
                executor->execute();
            }
        }
        catch (const std::invalid_argument e)
        {
            throw e;
        }
    }
    void ExecutorManager::shutdown_executor()
    {
        try
        {
            for (auto &executor : executors[ExecutorType::End])
            {
                executor->execute();
            }
        }
        catch (const std::invalid_argument e)
        {
            throw e;
        }
    }
}