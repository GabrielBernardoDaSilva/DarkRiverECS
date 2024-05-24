#include "lost_lands.hpp"

namespace forged_in_lost_lands_ecs
{
    void LostLands::remove_entity(Entity entity)
    {
        entity_manager.remove_entity(entity);
    }

    void LostLands::run()
    {
        executor_manager.execute();
    }
    std::vector<Archetype *> LostLands::create_archetype_ref()
    {
        std::vector<Archetype *> archetypes_ptr{};
        for (auto &archetype : entity_manager.archetypes)
        {
            archetypes_ptr.push_back(&archetype);
        }
        return archetypes_ptr;
    }

    void LostLands::show_archetypes() const
    {
        std::println("--------------------Archetypes-------------------");
        for (auto &archetype : entity_manager.archetypes)
        {
            archetype.list_all_components_hash();
        }
        std::println("------------------------------------------------");
    }
}
