#pragma once

namespace prometheus_ecs
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