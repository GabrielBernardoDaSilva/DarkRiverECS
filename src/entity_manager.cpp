#include "entity_manager.hpp"

namespace winter_rain_ecs
{
    std::expected<Success, ArchetypeError> EntityManager::remove_entity(Entity entity)
    {
        auto entity_founded = std::ranges::find_if(entities, [&](Entity &e)
                                                   { return e.id == entity.id; });
        if (entity_founded != entities.end())
        {
            std::size_t archetype_location = entity_founded->location;
            Archetype &archetype = archetypes[archetype_location];
            archetype.remove_entity(*entity_founded);
            entities.erase(entity_founded);
            return Success{};
        }
        return std::unexpected(ArchetypeError::EntityNotFound);
    }

} // namespace winter_rain_ecs