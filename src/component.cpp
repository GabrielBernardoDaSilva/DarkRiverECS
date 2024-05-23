#include "component.hpp"

namespace forged_in_lost_lands_ecs
{

    void ComponentList::add_component(std::unique_ptr<Component> &&component)
    {
        components.push_back(std::move(component));
    }

    std::unique_ptr<Component> forged_in_lost_lands_ecs::ComponentList::remove_component(std::size_t index)
    {
        std::unique_ptr<Component> component = std::move(components[index]);
        components.erase(components.begin() + index);
        return component;
    }
    std::unique_ptr<Component> &ComponentList::get_component(std::size_t index)
    {
        return components[index];
    }
    const std::size_t ComponentList::size() const
    {
        return components.size();
    }
}