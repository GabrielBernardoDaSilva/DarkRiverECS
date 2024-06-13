#pragma once
#include "archetype.hpp"
#include "component.hpp"
#include "custom_concepts.hpp"
#include "accessor.hpp"

#include <vector>
#include <tuple>
#include <ranges>
#include <algorithm>

namespace darkriver
{

    template <validation_query_types... T>
    struct With
    {
    };

    template <validation_query_types... T>
    struct Without
    {
    };

    class QueryBase
    {
    public:
        QueryBase() = default;

        virtual ~QueryBase() = default;

        virtual void execute() = 0;
    };

    template <typename WithComponent, typename Without = Without<>>
    class Query;

    template <typename... WithComponent, typename... WithoutComponents>
    class Query<With<WithComponent...>, Without<WithoutComponents...>> final : public QueryBase
    {
    public:
        explicit Query(Accessor &accessor) : m_accessor(accessor), m_archetypes(accessor.get_archetypes()) {
                                             };

        ~Query() override = default;

        template <typename Arg>
        inline constexpr Arg &get_component(ComponentList &components, const std::size_t index)
        {
            auto &component = *components.get_component(index);
            return dynamic_cast<ComponentWrapper<std::remove_const_t<std::remove_reference_t<Arg>>> &>(component).m_component;
        }

        inline constexpr void execute() override
        {
            for (Archetype &archetype : m_archetypes)
            {
                auto &components = archetype.get_components();
                auto &entities = archetype.get_entities();

                const bool is_archetype_components_bigger = m_with_components.size() > components.size();

                const bool is_archetype_empty = archetype.is_empty();

                const bool is_with_components = std::ranges::any_of(components | std::views::keys,
                                                                    [&](const std::size_t key)
                                                                    {
                                                                        return std::ranges::find(
                                                                                   m_with_components.begin(),
                                                                                   m_with_components.end(),
                                                                                   key) != m_with_components.end();
                                                                    });
                const bool is_without_components = std::ranges::any_of(components | std::views::keys,
                                                                       [&](const std::size_t key)
                                                                       {
                                                                           return std::ranges::find(
                                                                                      m_without_components.begin(),
                                                                                      m_without_components.end(),
                                                                                      key) != m_without_components.end();
                                                                       });

                if (is_with_components && !is_without_components && !is_archetype_components_bigger && !is_archetype_empty)
                {
                    for (std::size_t i = 0; i < entities.size(); i++)
                    {
                        if (((components.contains(typeid(WithComponent).hash_code())) && ...))
                        {
                            auto component_tuple = std::tie(get_component<WithComponent>(*components.at(
                                                                                             typeid(WithComponent).hash_code()),
                                                                                         i)...);
                            std::tuple<WithComponent...> tuple = std::tuple<WithComponent...>{std::get<WithComponent>(component_tuple)...};

                            m_result.push_back(tuple);
                        }
                    }
                }
            }
        }

        inline constexpr const std::optional<std::tuple<WithComponent...>> first()
        {
            execute();
            if (m_result.empty())
                return std::nullopt;
            return m_result.front();
        }

        inline constexpr const std::optional<std::tuple<WithComponent...>> last()
        {
            execute();
            if (m_result.empty())
                return std::nullopt;
            return m_result.back();
        }

        inline constexpr const std::vector<std::tuple<WithComponent...>> &all()
        {
            execute();
            return m_result;
        }

    private:
        std::vector<Archetype> &m_archetypes;
        std::vector<std::size_t> m_with_components{typeid(WithComponent).hash_code()...};
        std::vector<std::size_t> m_without_components{typeid(WithoutComponents).hash_code()...};
        std::vector<std::tuple<WithComponent...>> m_result{};
        Accessor &m_accessor;
    };

}
