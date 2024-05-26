#pragma once
#include "archetype.hpp"
#include "component.hpp"
#include "forged_concepts.hpp"
#include "accessor.hpp"

#include <vector>
#include <tuple>
#include <ranges>
#include <algorithm>

namespace prometheus_ecs
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
	class Query<With<WithComponent...>, Without<WithoutComponents...>> : public QueryBase
	{
	public:
		Query(Accessor &accessor) : accessor(accessor) {
									};
		~Query() = default;

		inline constexpr void execute() override
		{
			for (Archetype *archetype : archetypes)
			{
				auto &components = archetype->get_components();
				auto &entities = archetype->get_entities();

				bool is_archetype_components_bigger = with_components.size() > components.size();

				bool is_archetype_empty = archetype->is_empty();

				bool is_with_components = std::ranges::any_of(components | std::views::keys, [&](std::size_t key)
															  { return std::ranges::find(with_components.begin(), with_components.end(), key) != with_components.end(); });
				bool is_without_components = std::ranges::any_of(components | std::views::keys, [&](std::size_t key)
																 { return std::ranges::find(without_components.begin(), without_components.end(), key) != without_components.end(); });

				if (is_with_components && !is_without_components && !is_archetype_components_bigger && !is_archetype_empty)
				{
					for (std::size_t i = 0; i < entities.size(); i++)
					{
						if (((components.find(typeid(WithComponent).hash_code())) != components.end() && ...))
							{
								auto component_tuple = std::tie(*static_cast<std::remove_reference_t<WithComponent> *>(
									components[typeid(WithComponent).hash_code()]->get_component(i).get())...);
								std::tuple<WithComponent...> tuple = std::tuple<WithComponent...>{std::get<WithComponent>(component_tuple)...};
								result.push_back(tuple);
							}
					}
				}
			}
		}

		inline constexpr const std::optional<std::tuple<WithComponent...>> &first()
		{
			if (archetypes.size() != accessor.get_archetype_size())
				archetypes = accessor.get_archetypes();
			execute();
			if (result.empty())
				return std::nullopt;
			return result.front();
		}

		inline constexpr const std::optional<std::tuple<WithComponent...>> &last()
		{
			if (archetypes.size() != accessor.get_archetype_size())
				archetypes = accessor.get_archetypes();
			execute();
			if (result.empty())
				return std::nullopt;
			return result.back();
		}

		inline constexpr const std::vector<std::tuple<WithComponent...>> &all()
		{
			if (archetypes.size() != accessor.get_archetype_size())
				archetypes = accessor.get_archetypes();
			execute();
			return result;
		}

	private:
		std::vector<Archetype *> archetypes{};
		std::vector<std::size_t> with_components{typeid(WithComponent).hash_code()...};
		std::vector<std::size_t> without_components{typeid(WithoutComponents).hash_code()...};
		std::vector<std::tuple<WithComponent...>> result{};
		Accessor &accessor;
	};
}