#include "archetype.hpp"

#include <ranges>

namespace winter_rain_ecs
{
    Archetype::Archetype(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<BaseComponentWrapper>>> moved_entity)
    {
        auto &[entity, components] = moved_entity;
        entities.push_back(entity.id);
        for (auto &[key, value] : components)
        {
            auto list = std::make_unique<ComponentList>();
            list->add_component(std::move(value));
            this->components.insert({key, std::move(list)});
        }
    }
    bool Archetype::has_component_by_hash(const std::size_t hash) const
    {
        return components.contains(hash);
    }
    bool Archetype::has_components_by_hash(const std::vector<std::size_t> &hashes) const
    {
        return std::ranges::all_of(components | std::views::keys, [&hashes](const std::size_t hash)
                                   { return std::ranges::find(hashes, hash) != hashes.end(); }) && hashes.size() == components.size();
    }
    void Archetype::get_archetype_hash(std::vector<std::size_t> &hashes)
    {
        for (const auto &key : components | std::views::keys)
        {
            hashes.push_back(key);
        }
    }

    bool Archetype::is_empty() const
    {
        return entities.empty();
    }
    std::expected<Entity, ArchetypeError> Archetype::remove_entity(Entity entity)
    {
        if (const auto it = std::ranges::find_if(entities, [entity](EntityId id)
                                                 { return id == entity.id; });
            it != entities.end())
        {
            const auto index = std::distance(entities.begin(), it);
            // remove components from this entity
            for (const auto &value : components | std::views::values)
            {
                value->remove_component(index);
            }
            entities.erase(it);
            return entity;
        }
        return std::unexpected(ArchetypeError::EntityNotFound);
    }
    std::expected<std::tuple<Entity, std::map<std::size_t, std::unique_ptr<BaseComponentWrapper>>>, ArchetypeError> Archetype::move_entity(Entity entity)
    {
        if (const auto entity_it = std::ranges::find(entities, entity.id); entity_it != entities.end())
        {
            const auto index = std::distance(entities.begin(), entity_it);
            std::map<std::size_t, std::unique_ptr<BaseComponentWrapper>> component_to_migrate;
            for (auto &[key, value] : components)
            {
                component_to_migrate.insert({key, value->remove_component(index)});
            }
            entities.erase(entity_it);
            auto tuple = std::make_tuple(entity, std::move(component_to_migrate));
            return tuple;
        }
        return std::unexpected(ArchetypeError::EntityNotFound);
    }

    void Archetype::migrate_entity_to_itself(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<BaseComponentWrapper>>> tuple)
    {
        auto &[entity, components] = tuple;
        entities.push_back(entity.id);
        for (auto &[key, value] : components)
        {
            std::println("Migrating component with hash: {}", key);
            if (auto it = this->components.find(key); it != this->components.end())
            {
                it->second->add_component(std::move(value));
            }
        }
    }
    void Archetype::list_all_components_hash() const
    {
        std::println("Archetype has {} entities", entities.size());
        for (auto &[key, value] : components)
        {
            std::println("Component hash: {} and has {} components", key, value->size());
        }
    }
    std::map<std::size_t, std::unique_ptr<ComponentList>> &Archetype::get_components()
    {
        return components;
    }
    const std::vector<EntityId> &Archetype::get_entities() const
    {
        return entities;
    }

    std::string Archetype::get_name()
    {
        return name;
    }
    void Archetype::set_name(const char *name)
    {
        this->name = name;
    }
}
