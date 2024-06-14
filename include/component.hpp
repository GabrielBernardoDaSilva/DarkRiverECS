#pragma once

#include <vector>
#include <memory>
#include <cstddef>

namespace darkriver
{
    using ComponentId = std::size_t;

    class BaseComponentWrapper
    {
    public:
        virtual ~BaseComponentWrapper() = default;
    };

    template <typename T>
    struct ComponentWrapper final : public BaseComponentWrapper
    {
        explicit ComponentWrapper(T &&component) : m_component(std::forward<T>(component))
        {
        }

        T m_component;
    };

    class ComponentList
    {
    private:
        std::vector<std::unique_ptr<BaseComponentWrapper>> m_components;

    public:
        ComponentList() = default;

        ComponentList(ComponentList &list) = delete;

        ComponentList(ComponentList &&list) = delete;

        void add_component(std::unique_ptr<BaseComponentWrapper> &&component);

        std::unique_ptr<BaseComponentWrapper> remove_component(std::size_t index);

        std::unique_ptr<BaseComponentWrapper> &get_component(std::size_t index);

        [[nodiscard]] std::size_t size() const;
    };
}
