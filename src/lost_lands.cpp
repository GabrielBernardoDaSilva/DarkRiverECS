#include "lost_lands.hpp"

namespace forged_in_lost_lands_ecs
{
    void LostLands::remove_entity(Entity entity)
    {
        auto entity_founded = std::ranges::find_if(entities, [&](Entity &e)
                                                   { return e.id == entity.id; });
        if (entity_founded != entities.end())
        {
            std::size_t archetype_location = entity_founded->location;
            Archetype &archetype = archetypes[archetype_location];
            archetype.remove_entity(*entity_founded);
            entities.erase(entity_founded);
        }
    }

    void LostLands::run()
    {
        executor_manager.execute();
    }
    std::vector<Archetype *> LostLands::create_archetype_ref()
    {
        std::vector<Archetype *> archetypes_ptr{};
        for (auto &archetype : archetypes)
        {
            archetypes_ptr.push_back(&archetype);
        }
        return archetypes_ptr;
    }

    void LostLands::show_archetypes() const
    {
        std::println("--------------------Archetypes-------------------");
        for (auto &archetype : archetypes)
        {
            archetype.list_all_components_hash();
        }
        std::println("------------------------------------------------");
    }
}
