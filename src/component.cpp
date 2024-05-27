#include "component.hpp"

#include <print>
#include <memory>

namespace winter_rain_ecs
{

    void ComponentList::add_component(std::unique_ptr<BaseComponentWrapper> &&component)
    {
        components.push_back(std::move(component));
    }

    std::unique_ptr<BaseComponentWrapper> winter_rain_ecs::ComponentList::remove_component(const std::size_t index)
    {
        std::unique_ptr<BaseComponentWrapper> component = std::move(components[index]);
        components.erase(components.begin() + index);
        return component;
    }
    std::unique_ptr<BaseComponentWrapper> &ComponentList::get_component(const std::size_t index)
    {
        return components[index];
    }
    std::size_t ComponentList::size() const
    {
        return components.size();
    }
}