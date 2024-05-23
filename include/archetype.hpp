#pragma once
#include <cstddef>
#include <vector>
#include <map>
#include <concepts>
#include <memory>
#include <format>
#include <ranges>
#include <expected>
#include <print>
#include <string>
#include <ranges>
#include <algorithm>

#include "component.hpp"
#include "util.hpp"

namespace forged_in_lost_lands_ecs
{

	using EntityId = std::size_t;

	struct Entity : public Component
	{
		EntityId id;
		std::size_t location;
	};

	class Archetype
	{
	private:
		std::vector<EntityId> entities;
		std::map<ComponentId, std::unique_ptr<ComponentList>> components;
		std::string name{};

		template <class T>
		void add_component(T component)
		{
			const char *type = typeid(T).name();
			const std::size_t hash = get_hashed_id(type);
			if (components.find(hash) != components.end())
			{
				std::unique_ptr<Component> comp = std::make_unique<T>(component);
				components[hash]->add_component(std::move(comp));
				return;
			}
			// append name
			name.append(type);

			std::unique_ptr<ComponentList> list = std::make_unique<ComponentList>();
			std::unique_ptr<Component> comp = std::make_unique<T>(component);
			list->add_component(std::move(comp));
			components.insert({hash, std::move(list)});
		}
		template <class... T>
		void add_components(T... component)
		{
			(add_component(component), ...);
		}

	public:
		template <typename... T>
		Archetype(Entity entity, T... components)
		{
			entities.push_back(entity.id);
			add_components<T...>(components...);
		}

		Archetype(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>> moved_entity);

		Archetype(Archetype &archetype) = delete;
		Archetype(Archetype &&archetype) = default;

		~Archetype() = default;

		template <class U>
		bool has_component()
		{
			const char *type = typeid(U).name();
			const std::size_t hash = get_hashed_id(type);
			return components.find(hash) != components.end();
		}

		template <class... U>
		bool has_components()
		{
			return (has_component<U> && ...);
		}

		template <typename... T>
		void add_entity(Entity entity, T... components)
		{
			entities.push_back(entity.id);
			add_components<T...>(components...);
		}

		std::expected<Entity, archetype_error> remove_entity(Entity entity);

		template <class U>
		std::expected<std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>>, archetype_error> remove_component(Entity entity)
		{
			const char *type = typeid(U).name();
			const std::size_t hash = get_hashed_id(type);

			auto entity_it = std::ranges::find(entities, entity.id);
			const bool has_component = this->has_component<U>();
			if (has_component && entity_it != entities.end())
			{
				// find component list
				auto it = components.find(hash);
				// find entity in archetype
				std::println("Removing component with hash: {}", it->first);
				auto index = std::distance(entities.begin(), entity_it);
				it->second->remove_component(index);
				// now we need to prepare other components to migrate this entity to other archetype
				std::map<std::size_t, std::unique_ptr<Component>> component_to_migrate;
				for (auto &[key, value] : components)
				{
					if (key != hash)
					{
						component_to_migrate.insert({key, value->remove_component(index)});
					}
				}
				// remove entity from this archetype
				entities.erase(entity_it);
				// make a tuple with the components to migrate and entity
				std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>> tuple = std::make_tuple(entity, std::move(component_to_migrate));

				return tuple;
			}
			return std::unexpected(archetype_error::COMPONENT_NOT_FOUND);
		}

		bool has_component_by_hash(std::size_t hash);

		bool has_components_by_hash(const std::vector<std::size_t> &hashes);

		void get_archetype_hash(std::vector<std::size_t> &hashes);

		bool is_empty() const;

		std::expected<std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>>, archetype_error> move_entity(Entity entity);

		void migrate_entity_to_itself(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>> tuple);

		void list_all_components_hash() const;

		std::map<std::size_t, std::unique_ptr<ComponentList>> &get_components();

		const std::vector<EntityId> &get_entities() const;

		std::string get_name();

		void set_name(const char *name);
	};

}