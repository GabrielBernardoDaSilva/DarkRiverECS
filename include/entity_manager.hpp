#pragma once
#include "custom_concepts.hpp"
#include "archetype.hpp"
#include "errors.hpp"

#include <expected>
#include <cstddef>
#include <typeinfo>

namespace darkriver
{
    class World;

    class EntityManager
    {
        friend class World;

    public:
        EntityManager() = default;

        ~EntityManager() = default;

        template <typename... T>
        Entity add_entity(T... components)
        {
            Entity entity{
                .id = m_entity_count++,
            };

            std::vector<std::size_t> component_hashes = {typeid(T).hash_code()...};
            component_hashes.emplace(component_hashes.begin(), typeid(Entity).hash_code());

            auto archetypeFounded = std::ranges::find_if(m_archetypes, [&](Archetype &archetype)
                                                         { return archetype.has_components_by_hash(component_hashes); });

            // found archetype
            if (archetypeFounded != m_archetypes.end())
            {
                entity.location = std::distance(m_archetypes.begin(), archetypeFounded);
                archetypeFounded->add_entity(entity, entity, std::forward<T>(components)...);
            }
            else
            {
                entity.location = m_archetypes.size();
                Archetype archetype{entity, entity,  std::forward<T>(components)...};
                m_archetypes.push_back(std::move(archetype));
            }

            m_entities.push_back(entity);

            return entity;
        };

        template <typename T>
        std::expected<Success, ArchetypeError> add_component_to_entity(const Entity entity, T component)
        {
            auto entity_founded = std::ranges::find_if(m_entities, [&](const Entity &e)
                                                       { return e.id == entity.id; });
            if (entity_founded != m_entities.end())
            {
                const std::size_t archetype_location = entity_founded->location;
                Archetype &old_archetype = m_archetypes[archetype_location];

                if (old_archetype.has_component<T>())
                    return std::unexpected(ArchetypeError::EntityAlreadyHasComponent);
                // need to move component to archetype or create new archetype
                auto moved_entity = old_archetype.move_entity(*entity_founded);
                if (!moved_entity.has_value())
                    return std::unexpected(moved_entity.error());
                std::vector<std::size_t> component_hashes = {};
                old_archetype.get_archetype_hash(component_hashes);
                component_hashes.push_back(typeid(T).hash_code());

                auto archetype_founded = std::ranges::find_if(m_archetypes, [&](const Archetype &arch)
                                                              { return arch.has_components_by_hash(component_hashes); });

                auto &[entity, components] = moved_entity.value();
                components.insert(std::make_pair(typeid(T).hash_code(), std::make_unique<ComponentWrapper<T>>(std::move(component))));

                // found archetype
                if (archetype_founded != m_archetypes.end())
                {
                    entity_founded->location = std::distance(m_archetypes.begin(), archetype_founded);
                    archetype_founded->migrate_entity_to_itself(std::move(moved_entity.value()));
                }
                else
                {
                    entity_founded->location = m_archetypes.size();
                    Archetype arch{std::move(moved_entity.value())};
                    m_archetypes.push_back(std::move(arch));
                }
                //
                remove_archetype_if_needed(archetype_location);
                return Success{};
            }

            return std::unexpected(ArchetypeError::EntityNotFound);
        }

        template <typename T>
        std::expected<Success, ArchetypeError> remove_component_from_entity(const Entity entity)
        {
            auto entity_founded = std::ranges::find_if(m_entities, [&](const Entity &e)
                                                       { return e.id == entity.id; });

            if (entity_founded != m_entities.end())
            {
                const std::size_t archetype_location = entity_founded->location;
                Archetype &old_archetype = m_archetypes[archetype_location];
                std::println("Removing component {}", typeid(T).hash_code());
                if (!old_archetype.has_component<T>())
                    return std::unexpected(ArchetypeError::EntityDoesNotHaveComponent);

                auto moved_entity = old_archetype.remove_component<T>(*entity_founded);
                if (!moved_entity.has_value())
                    return std::unexpected(moved_entity.error());
                std::vector<std::size_t> component_hashes = {};

                auto &[entity, components] = moved_entity.value();
                std::ranges::for_each(components, [&](auto &component)
                                      { component_hashes.push_back(component.first); });

                auto archetype_founded = std::ranges::find_if(m_archetypes, [&](const Archetype &archetype)
                                                              { return archetype.has_components_by_hash(component_hashes); });
                // found archetype
                if (archetype_founded != m_archetypes.end())
                {
                    entity_founded->location = std::distance(m_archetypes.begin(), archetype_founded);
                    archetype_founded->migrate_entity_to_itself(std::move(moved_entity.value()));
                }
                else
                {
                    entity_founded->location = m_archetypes.size();
                    Archetype archetype{std::move(moved_entity.value())};
                    m_archetypes.push_back(std::move(archetype));
                }
                remove_archetype_if_needed(archetype_location);
                return Success{};
            }

            return std::unexpected(ArchetypeError::EntityNotFound);
        }

        [[nodiscard]] std::size_t get_archetype_size() const
        {
            return m_archetypes.size();
        }

        std::expected<Success, ArchetypeError> remove_entity(Entity entity);

    private:
        std::vector<Archetype> m_archetypes{};
        std::vector<Entity> m_entities{};
        std::size_t m_entity_count{0};

        void remove_archetype_if_needed(const std::size_t archetype_location);
    };
} // namespace darkriver
