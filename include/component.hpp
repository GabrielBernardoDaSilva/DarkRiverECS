#pragma once

#include <vector>
#include <memory>
#include <cstddef>
#include <concepts>
#include <map>
#include <optional>
#include <print>

namespace forged_in_lost_lands_ecs
{

    using ComponentId = std::size_t;

    class Component
    {
    };

    struct Position : public Component
    {
        float x, y;
    };

    class ComponentList
    {
    private:
        std::vector<std::unique_ptr<Component>> components;

    public:
        ComponentList()
        {
        }
        ComponentList(ComponentList &list) = delete;
        ComponentList(ComponentList &&list) = delete;

        void add_component(std::unique_ptr<Component> &&component)
        {
            components.push_back(std::move(component));
        }

        std::unique_ptr<Component> remove_component(std::size_t index)
        {
            std::unique_ptr<Component> component = std::move(components[index]);
            components.erase(components.begin() + index);
            return component;
        }

        std::unique_ptr<Component> &get_component(std::size_t index)
        {
            return components[index];
        }

        const std::size_t size() const
        {
            return components.size();
        }
    };
}