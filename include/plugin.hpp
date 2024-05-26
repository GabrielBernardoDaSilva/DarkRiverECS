#pragma once

namespace winter_rain_ecs
{
    class World;
    class Plugin
    {
    public:
        Plugin() = default;
        virtual ~Plugin() = default;

        virtual void build(World &world) = 0;
    };
}