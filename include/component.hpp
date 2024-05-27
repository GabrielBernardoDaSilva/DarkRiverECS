#pragma once

#include <vector>
#include <memory>
#include <cstddef>


namespace winter_rain_ecs {
    using ComponentId = std::size_t;

    class Component {
    };

    class ComponentList {
    private:
        std::vector<std::unique_ptr<Component> > components;

    public:
        ComponentList() = default;

        ComponentList(ComponentList &list) = delete;

        ComponentList(ComponentList &&list) = delete;

        void add_component(std::unique_ptr<Component> &&component);


        std::unique_ptr<Component> remove_component(std::size_t index);


        std::unique_ptr<Component> &get_component(std::size_t index);


        [[nodiscard]]  std::size_t size() const;
    };
}
