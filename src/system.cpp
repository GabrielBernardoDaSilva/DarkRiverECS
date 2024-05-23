#include "system.hpp"
#include "lost_lands.hpp"

namespace forged_in_lost_lands_ecs
{
    void ExecutorManager::execute()
    {
        for (auto &executor : executors)
        {
            executor->execute();
        }
    }
}