#pragma once

namespace forged_in_lost_lands_ecs
{
    class LostLands;
    class Plugin
    {
    public:
        Plugin() = default;
        virtual ~Plugin() = default;

        virtual void build(LostLands &lost_land) = 0;
    };
}