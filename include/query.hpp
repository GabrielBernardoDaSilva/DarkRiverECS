#pragma once
#include "archetype.hpp"
#include "util.hpp"
#include "component.hpp"
#include "forged_concepts.hpp"
#include "accessor.hpp"

#include <vector>
#include <tuple>
#include <ranges>
#include <algorithm>

namespace forged_in_lost_lands_ecs
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

	template <typename T, typename Without = Without<>>
	class Query;

	template <typename... T, typename... WithoutComponents>
	class Query<With<T...>, Without<WithoutComponents...>> : public QueryBase
	{
	public:
		Query(Accessor &accessor) : accessor(accessor) {
									};
		~Query() = default;

		constexpr inline void execute() override
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
						if ((components.find(get_hashed_id(typeid(T).name())) != components.end() && ...))
						{
							auto component_tuple = std::tie(*static_cast<std::remove_reference_t<T> *>(
								components[get_hashed_id(typeid(T).name())]->get_component(i).get())...);
							std::tuple<T...> tuple = std::tuple<T...>{std::get<T>(component_tuple)...};
							result.push_back(tuple);
						}
					}
				}
			}
		}

		inline constexpr const std::tuple<T...> &first()
		{
			if (archetypes.size() != accessor.get_archetype_size())
				archetypes = accessor.get_archetypes();
			execute();
			return result.front();
		}

		inline constexpr const std::tuple<T...> &last()
		{
			if (archetypes.size() != accessor.get_archetype_size())
				archetypes = accessor.get_archetypes();
			execute();
			return result.back();
		}

		inline constexpr const std::vector<std::tuple<T...>> &all()
		{
			if (archetypes.size() != accessor.get_archetype_size())
				archetypes = accessor.get_archetypes();
			execute();
			return result;
		}

	private:
		std::vector<Archetype *> archetypes{};
		std::vector<std::size_t> with_components{get_hashed_id(typeid(T).name())...};
		std::vector<std::size_t> without_components{get_hashed_id(typeid(WithoutComponents).name())...};
		std::vector<std::tuple<T...>> result{};
		Accessor &accessor;
	};
}