#include "component.hpp"

#include <print>
#include <memory>

namespace winter_rain_ecs
{

    void ComponentList::add_component(std::unique_ptr<BaseComponentWrapper> &&component)
    {
        m_components.push_back(std::move(component));
    }

    std::unique_ptr<BaseComponentWrapper> winter_rain_ecs::ComponentList::remove_component(const std::size_t index)
    {
        std::unique_ptr<BaseComponentWrapper> component = std::move(m_components[index]);
        m_components.erase(m_components.begin() + index);
        return component;
    }
    std::unique_ptr<BaseComponentWrapper> &ComponentList::get_component(const std::size_t index)
    {
        return m_components[index];
    }
    std::size_t ComponentList::size() const
    {
        return m_components.size();
    }
}