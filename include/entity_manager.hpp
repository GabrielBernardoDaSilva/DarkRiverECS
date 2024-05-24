#pragma once
#include "forged_concepts.hpp"
#include "archetype.hpp"

namespace forged_in_lost_lands_ecs
{
    class LostLands;
    class EntityManager
    {
        friend class LostLands;
    public:
        EntityManager() = default;
        ~EntityManager() = default;

        template <req_component... T>
        Entity add_entity(T... components)
        {
            Entity entity{
                .id = entity_count++,
            };

            std::vector<std::size_t> component_hashes = {get_hashed_id(typeid(T).name())...};
            component_hashes.emplace(component_hashes.begin(), get_hashed_id(typeid(Entity).name()));

            auto archetypeFounded = std::ranges::find_if(archetypes, [&](Archetype &archetype)
                                                         { return archetype.has_components_by_hash(component_hashes); });

            // found archetype
            if (archetypeFounded != archetypes.end())
            {
                entity.location = std::distance(archetypes.begin(), archetypeFounded);
                archetypeFounded->add_entity(entity, entity, components...);
            }
            else
            {
                entity.location = archetypes.size();
                Archetype archetype{entity, entity, components...};
                archetypes.push_back(std::move(archetype));
            }

            entities.push_back(entity);

            return entity;
        };

        template <req_component T>
        void add_component_to_entity(Entity entity, T component)
        {
            auto entity_founded = std::ranges::find_if(entities, [&](Entity &e)
                                                       { return e.id == entity.id; });
            if (entity_founded != entities.end())
            {

                std::size_t archetype_location = entity_founded->location;
                Archetype &archetype = archetypes[archetype_location];
                // need to move component to archetype or create new archetype
                std::expected<std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>>, archetype_error> moved_entity = archetype.move_entity(*entity_founded);
                std::vector<std::size_t> component_hashes = {};
                archetype.get_archetype_hash(component_hashes);
                component_hashes.push_back(get_hashed_id(typeid(T).name()));

                auto archetype_founded = std::ranges::find_if(archetypes, [&](Archetype &archetype)
                                                              { return archetype.has_components_by_hash(component_hashes); });

                auto &[entity, components] = moved_entity.value();
                components.insert(std::make_pair(get_hashed_id(typeid(T).name()), std::make_unique<T>(std::move(component))));

                // found archetype
                if (archetype_founded != archetypes.end())
                {
                    entity_founded->location = std::distance(archetypes.begin(), archetype_founded);
                    archetype_founded->migrate_entity_to_itself(std::move(moved_entity.value()));
                }
                else
                {
                    entity_founded->location = archetypes.size();
                    Archetype archetype{std::move(moved_entity.value())};
                    archetypes.push_back(std::move(archetype));
                }
            }
        }

        template <req_component T>
        void remove_component_from_entity(Entity entity)
        {
            auto entity_founded = std::ranges::find_if(entities, [&](Entity &e)
                                                       { return e.id == entity.id; });
            if (entity_founded != entities.end())
            {
                std::size_t archetype_location = entity_founded->location;
                Archetype &old_archetype = archetypes[archetype_location];
                std::expected<std::tuple<Entity, std::map<std::size_t, std::unique_ptr<Component>>>, archetype_error> moved_entity = old_archetype.remove_component<T>(*entity_founded);
                std::vector<std::size_t> component_hashes = {};

                auto &[entity, components] = moved_entity.value();
                std::ranges::for_each(components, [&](auto &component)
                                      {
					std::println("Component hash: {}", component.first);
					component_hashes.push_back(component.first); });

                auto archetype_founded = std::ranges::find_if(archetypes, [&](Archetype &archetype)
                                                              { return archetype.has_components_by_hash(component_hashes); });
                // found archetype
                if (archetype_founded != archetypes.end())
                {
                    entity_founded->location = std::distance(archetypes.begin(), archetype_founded);
                    archetype_founded->migrate_entity_to_itself(std::move(moved_entity.value()));
                    std::println("Migrated entity to archetype: {}", entity.location);
                }
                else
                {
                    entity_founded->location = archetypes.size();
                    Archetype archetype{std::move(moved_entity.value())};
                    archetypes.push_back(std::move(archetype));
                    std::println("Created new archetype: {}", entity.location);
                }
            }
        }

        void remove_entity(Entity entity);

    private:
        std::vector<Archetype> archetypes{};
        std::vector<Entity> entities{};
        std::size_t entity_count{0};
    };
} // namespace forged_in_lost_lands_ecs