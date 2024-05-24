#include "system.hpp"
#include "lost_lands.hpp"

#include <iostream>
#include <exception>

namespace forged_in_lost_lands_ecs
{
    void ExecutorManager::execute()
    {
        try
        {
            for (auto &executor : executors)
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