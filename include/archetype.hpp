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

namespace forged_in_lost_lands_ecs {

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
		void add_component(T component) {
			const char* type = typeid(T).name();
			const std::size_t hash = get_hashed_id(type);
			if (components.find(hash) != components.end()) {
				std::unique_ptr<Component> comp = std::make_unique<T>(component);
				components[hash]->add_component(std::move(comp));
				return;
			}
			// append name
			name.append(type);

			std::unique_ptr<ComponentList> list = std::make_unique<ComponentList>();
			std::unique_ptr<Component> comp = std::make_unique<T>(component);
			list->add_component(std::move(comp));
			components.insert({ hash,  std::move(list) });
		}
		template <class ...T>
		void add_components(T... component) {
			(add_component(component), ...);
		}


	public:
		template <typename ...T>
		Archetype(Entity entity, T... components) {
			entities.push_back(entity.id);
			add_components<T...>(components...);
		}

		Archetype(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>> moved_entity) {
			auto& [entity, components] = moved_entity;
			entities.push_back(entity.id);
			for (auto& [key, value] : components) {
				std::unique_ptr<ComponentList> list = std::make_unique<ComponentList>();
				list->add_component(std::move(value));
				this->components.insert({ key, std::move(list) });
			}
		}

		Archetype(Archetype& archetype) = delete;
		Archetype(Archetype&& archetype) = default;

		~Archetype() {
			entities.clear();
			components.clear();
		}

		template <class U>
		bool has_component() {
			const char* type = typeid(U).name();
			const std::size_t hash = get_hashed_id(type);
			return components.find(hash) != components.end();
		}

		template <class ...U>
		bool has_components() {
			return (has_component<U> &&...);
		}



		bool has_component_by_hash(std::size_t hash) {
			return components.find(hash) != components.end();
		}

		bool has_components_by_hash(const std::vector<std::size_t>& hashes) {
			return std::ranges::all_of(hashes, [this](std::size_t hash) {
				return has_component_by_hash(hash);
				});
		}

		void get_archetype_hash(std::vector<std::size_t>& hashes) {
			for (auto& [key, _] : components) {
				hashes.push_back(key);
			}
		}

		auto is_empty() -> bool {
			return entities.size() == 0;
		}


		template <typename ...T>
		void add_entity(Entity entity, T... components) {
			entities.push_back(entity.id);
			add_components<T...>(components...);
		}

		std::expected<Entity, archetype_error> remove_entity(Entity entity) {
			auto it = std::ranges::find_if(entities, [entity](EntityId id) { return id == entity.id; });
			if (it != entities.end()) {
				auto index = std::distance(entities.begin(), it);
				// remove components from this entity
				for (auto& [_, value] : components) {
					value->remove_component(index);
				}
				entities.erase(it);
				return entity;
			}
			return std::unexpected(archetype_error::ENTITY_NOT_FOUND);
		}


		template <class U>
		std::expected<std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>>, archetype_error> remove_component(Entity entity) {
			const char* type = typeid(U).name();
			const std::size_t hash = get_hashed_id(type);

			auto entity_it = std::ranges::find(entities, entity.id);
			const bool has_component = this->has_component<U>();
			if (has_component && entity_it != entities.end()) {
				// find component list
				auto it = components.find(hash);
				// find entity in archetype
				std::println("Removing component with hash: {}", it->first);
				auto index = std::distance(entities.begin(), entity_it);
				it->second->remove_component(index);
				// now we need to prepare other components to migrate this entity to other archetype
				std::map<std::size_t, std::unique_ptr<Component>> component_to_migrate;
				for (auto& [key, value] : components) {
					if (key != hash) {
						component_to_migrate.insert({ key, value->remove_component(index) });
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

		std::expected<std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>>, archetype_error> move_entity(Entity entity) {
			auto entity_it = std::ranges::find(entities, entity.id);
			if (entity_it != entities.end()) {
				auto index = std::distance(entities.begin(), entity_it);
				std::map<std::size_t, std::unique_ptr<Component>> component_to_migrate;
				for (auto& [key, value] : components) {
					component_to_migrate.insert({ key, value->remove_component(index) });
				}
				entities.erase(entity_it);
				std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>> tuple = std::make_tuple(entity, std::move(component_to_migrate));
				return tuple;
			}
			return std::unexpected(archetype_error::ENTITY_NOT_FOUND);
		}

		void migrate_entity_to_itself(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>> tuple) {
			auto& [entity, components] = tuple;
			entities.push_back(entity.id);
			for (auto& [key, value] : components) {
				std::println("Migrating component with hash: {}", key);
				auto it = this->components.find(key);
				if (it != this->components.end()) {
					it->second->add_component(std::move(value));
				}
			}
		}

		inline void list_all_components_hash() {
			std::println("Archetype has {} entities", entities.size());
			for (auto& [key, value] : components) {
				std::println("Component hash: {} and has {} components", key, value->size());
			}
		}

		std::map<std::size_t, std::unique_ptr<ComponentList>>& get_components() {
			return components;
		}

		const std::vector<EntityId>& get_entities() const {
			return entities;
		}

		std::string get_name() {
			return name;
		}

		void set_name(const char* name) {
			this->name = name;
		}
	};

}