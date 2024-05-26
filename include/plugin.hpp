#pragma once

namespace forged_in_lost_lands_ecs
{
    class World;
    class Plugin
    {
    public:
        Plugin() = default;
        virtual ~Plugin() = default;

        virtual void build(World &lost_land) = 0;
    };
}