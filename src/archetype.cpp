#include "archetype.hpp"

#include <ranges>

namespace winter_rain_ecs
{
    Archetype::Archetype(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<BaseComponentWrapper>>> moved_entity)
    {
        auto &[entity, components] = moved_entity;
        m_entities.push_back(entity.id);
        for (auto &[key, value] : components)
        {
            auto list = std::make_unique<ComponentList>();
            list->add_component(std::move(value));
            this->m_components.insert({key, std::move(list)});
        }
    }
    bool Archetype::has_component_by_hash(const std::size_t hash) const
    {
        return m_components.contains(hash);
    }
    bool Archetype::has_components_by_hash(const std::vector<std::size_t> &hashes) const
    {
        return std::ranges::all_of(m_components | std::views::keys, [&hashes](const std::size_t hash)
                                   { return std::ranges::find(hashes, hash) != hashes.end(); }) && hashes.size() == m_components.size();
    }
    void Archetype::get_archetype_hash(std::vector<std::size_t> &hashes)
    {
        for (const auto &key : m_components | std::views::keys)
        {
            hashes.push_back(key);
        }
    }

    bool Archetype::is_empty() const
    {
        return m_entities.empty();
    }
    std::expected<Entity, ArchetypeError> Archetype::remove_entity(Entity entity)
    {
        if (const auto it = std::ranges::find_if(m_entities, [entity](EntityId id)
                                                 { return id == entity.id; });
            it != m_entities.end())
        {
            const auto index = std::distance(m_entities.begin(), it);
            // remove components from this entity
            for (const auto &value : m_components | std::views::values)
            {
                value->remove_component(index);
            }
            m_entities.erase(it);
            return entity;
        }
        return std::unexpected(ArchetypeError::EntityNotFound);
    }
    std::expected<std::tuple<Entity, std::map<std::size_t, std::unique_ptr<BaseComponentWrapper>>>, ArchetypeError> Archetype::move_entity(Entity entity)
    {
        if (const auto entity_it = std::ranges::find(m_entities, entity.id); entity_it != m_entities.end())
        {
            const auto index = std::distance(m_entities.begin(), entity_it);
            std::map<std::size_t, std::unique_ptr<BaseComponentWrapper>> component_to_migrate;
            for (auto &[key, value] : m_components)
            {
                component_to_migrate.insert({key, value->remove_component(index)});
            }
            m_entities.erase(entity_it);
            auto tuple = std::make_tuple(entity, std::move(component_to_migrate));
            return tuple;
        }
        return std::unexpected(ArchetypeError::EntityNotFound);
    }

    void Archetype::migrate_entity_to_itself(std::tuple<Entity, std::map<std::size_t, std::unique_ptr<BaseComponentWrapper>>> tuple)
    {
        auto &[entity, components] = tuple;
        m_entities.push_back(entity.id);
        for (auto &[key, value] : components)
        {
            std::println("Migrating component with hash: {}", key);
            if (auto it = this->m_components.find(key); it != this->m_components.end())
            {
                it->second->add_component(std::move(value));
            }
        }
    }
    void Archetype::list_all_components_hash() const
    {
        std::println("Archetype has {} entities", m_entities.size());
        for (auto &[key, value] : m_components)
        {
            std::println("Component hash: {} and has {} components", key, value->size());
        }
    }
    std::map<std::size_t, std::unique_ptr<ComponentList>> &Archetype::get_components()
    {
        return m_components;
    }
    const std::vector<EntityId> &Archetype::get_entities() const
    {
        return m_entities;
    }

}
