#ifndef LOST_LANDS_HPP
#define LOST_LANDS_HPP
#include <vector>
#include <memory>
#include <cstddef>
#include <ranges>
#include <print>
#include <expected>
#include <functional>
#include <iostream>
#include <type_traits>
#include <tuple>
#include <string_view>

#include "archetype.hpp"
#include "system.hpp"
#include "forged_concepts.hpp"
#include "errors.hpp"
#include "util.hpp"
#include "system.hpp"
#include "event.hpp"
#include "plugin.hpp"
#include "scheduler.hpp"
#include "generator.hpp"

namespace forged_in_lost_lands_ecs
{
    class LostLands
    {
    private:
        std::vector<Archetype> archetypes{};
        std::vector<Entity> entities{};
        std::vector<std::unique_ptr<Plugin>> plugins{};
        std::size_t entity_count{};
        ExecutorManager executor_manager{};
        EventManager event_manager{};
        TaskManager task_manager{};

    public:
        LostLands() = default;
        ~LostLands() = default;

        /// archetypes

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

        /// archetypes end

        /// systems

        template <typename... Args>
        void add_executor(std::function<void(Args...)> system)
        {
            executor_manager.add_executor(system, create_archetype_ref());
        }

        template <typename... Args>
        void add_executor(void (*system)(Args...))
        {
            executor_manager.add_executor(system, create_archetype_ref());
        }

        template <validation_query_types... T>
        Query<T...> query()
        {
            return Query<T...>{create_archetype_ref()};
        }

        void run();

        ///

        /// events

        template <req_event_ty Ev>
        void emit(Ev event)
        {
            event_manager.emit(event);
        }

        template <req_event_ty Ev>
        void subscribe(std::function<void(Ev)> subscriber)
        {
            std::cout << "Function name: " << (FUNCTION_TRAITS(subscriber)) << std::endl;
            event_manager.subscribe<Ev>(subscriber);
        }

        template <req_event_ty Ev>
        void subscribe(void (*subscriber)(Ev))
        {
            std::cout << "Function name: " << (FUNCTION_TRAITS_NAME(subscriber)) << std::endl;
            event_manager.subscribe<Ev>(subscriber);
        }

        template <req_event_ty Ev>
        void unsubscribe(std::function<void(Ev)> subscriber)
        {
            event_manager.unsubscribe<Ev>(subscriber);
        }

        template <req_event_ty Ev>
        void unsubscribe(void (*subscriber)(Ev))
        {
            event_manager.unsubscribe<Ev>(subscriber);
        }

        // plugins
        template <req_plugin T>
        void add_plugin(T &&plugin)
        {
            plugins.push_back(std::make_unique<T>(std::move(plugin)));
        }
        //

        // tasks
        template <typename... Args>
        TaskId add_task(std::function<generator<WaitAmountOfSeconds>(Args...)> task, Args... args)
        {
            return task_manager.add_task({task, args...});
        }

        template <typename... Args>
        TaskId add_task(generator<WaitAmountOfSeconds> (*task)(Args...), Args... args)
        {
            return task_manager.add_task({task, args...});
        }

        void run_tasks(float dt)
        {
            task_manager.execute_all(dt);
        }

        void resume_task(TaskId id)
        {
            task_manager.resume_task(id);
        }

        void stop_task(TaskId id)
        {
            task_manager.stop_task(id);
        }

        void remove_task(TaskId id)
        {
            task_manager.remove_task(id);
        }

        void stop_all_tasks()
        {
            task_manager.stop_all_tasks();
        }

        void resume_all_tasks()
        {
            task_manager.resume_all_tasks();
        }

        std::vector<Archetype *> create_archetype_ref();
        void show_archetypes() const;
    };
}

#endif // LOST_LANDS_HPP