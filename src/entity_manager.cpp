#include "entity_manager.hpp"

namespace darkriver
{
    void EntityManager::remove_archetype_if_needed(const std::size_t archetype_location)
    {
        Archetype &archetype = m_archetypes[archetype_location];
        if (archetype.is_empty() && m_archetypes.size() > 50)
        {
            m_archetypes.erase(m_archetypes.begin() + archetype_location);
            for (auto &entity : m_entities)
            {
                if (entity.location > archetype_location)
                    entity.location--;
            }
        }
    }

    std::expected<Success, ArchetypeError> EntityManager::remove_entity(const Entity entity)
    {
        if (const auto entity_founded = std::ranges::find_if(m_entities, [&](Entity &e)
                                                             { return e.id == entity.id; });
            entity_founded != m_entities.end())
        {
            const std::size_t archetype_location = entity_founded->location;
            Archetype &archetype = m_archetypes[archetype_location];
            if (const auto ret = archetype.remove_entity(*entity_founded); !ret.has_value())
                return std::unexpected(ret.error());
            m_entities.erase(entity_founded);
            return Success{};
        }
        return std::unexpected(ArchetypeError::EntityNotFound);
    }
} // namespace darkriver
