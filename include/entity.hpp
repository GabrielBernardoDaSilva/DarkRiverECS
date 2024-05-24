#pragma once

namespace forged_in_lost_lands_ecs
{
    class LostLands;
    class EntityManager
    {
    public:
        EntityManager(LostLands& lost_land) : lost_land(lost_land) {}
        ~EntityManager() = default;
    private:
        LostLands& lost_land;

    };
} // namespace forged_in_lost_lands_ecs
