#include "entity_manager.hpp"

namespace winter_rain_ecs {
    std::expected<Success, ArchetypeError> EntityManager::remove_entity(const Entity entity) {
        if (const auto entity_founded = std::ranges::find_if(entities, [&](Entity &e) { return e.id == entity.id; });
            entity_founded != entities.end()) {
            const std::size_t archetype_location = entity_founded->location;
            Archetype &archetype = archetypes[archetype_location];
            if (const auto ret = archetype.remove_entity(*entity_founded); !ret.has_value())
                return std::unexpected(ret.error());
            entities.erase(entity_founded);
            return Success{};
        }
        return std::unexpected(ArchetypeError::EntityNotFound);
    }
} // namespace winter_rain_ecs
