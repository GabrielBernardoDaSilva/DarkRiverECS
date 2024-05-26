#include "archetype.hpp"

namespace winter_rain_ecs
{
    Archetype::Archetype(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>> moved_entity)
    {
        auto &[entity, components] = moved_entity;
        entities.push_back(entity.id);
        for (auto &[key, value] : components)
        {
            std::unique_ptr<ComponentList> list = std::make_unique<ComponentList>();
            list->add_component(std::move(value));
            this->components.insert({key, std::move(list)});
        }
    }
    bool Archetype::has_component_by_hash(std::size_t hash)
    {
        return components.find(hash) != components.end();
    }
    bool Archetype::has_components_by_hash(const std::vector<std::size_t> &hashes)
    {
        return std::ranges::all_of(hashes, [this](std::size_t hash)
                                   { return has_component_by_hash(hash); });
    }
    void Archetype::get_archetype_hash(std::vector<std::size_t> &hashes)
    {
        for (auto &[key, _] : components)
        {
            hashes.push_back(key);
        }
    }

    bool Archetype::is_empty() const
    {
        return entities.size() == 0;
    }
    std::expected<Entity, ArchetypeError> Archetype::remove_entity(Entity entity)
    {
        auto it = std::ranges::find_if(entities, [entity](EntityId id)
                                       { return id == entity.id; });
        if (it != entities.end())
        {
            auto index = std::distance(entities.begin(), it);
            // remove components from this entity
            for (auto &[_, value] : components)
            {
                value->remove_component(index);
            }
            entities.erase(it);
            return entity;
        }
        return std::unexpected(ArchetypeError::EntityNotFound);
    }
    std::expected<std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>>, ArchetypeError> Archetype::move_entity(Entity entity)
    {
        auto entity_it = std::ranges::find(entities, entity.id);
        if (entity_it != entities.end())
        {
            auto index = std::distance(entities.begin(), entity_it);
            std::map<std::size_t, std::unique_ptr<Component>> component_to_migrate;
            for (auto &[key, value] : components)
            {
                component_to_migrate.insert({key, value->remove_component(index)});
            }
            entities.erase(entity_it);
            std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>> tuple = std::make_tuple(entity, std::move(component_to_migrate));
            return tuple;
        }
        return std::unexpected(ArchetypeError::EntityNotFound);
    }

    void Archetype::migrate_entity_to_itself(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>> tuple)
    {
        auto &[entity, components] = tuple;
        entities.push_back(entity.id);
        for (auto &[key, value] : components)
        {
            std::println("Migrating component with hash: {}", key);
            auto it = this->components.find(key);
            if (it != this->components.end())
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