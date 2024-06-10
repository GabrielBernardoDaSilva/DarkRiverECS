#pragma once
#include "forged_concepts.hpp"
#include "accessor.hpp"

#include <functional>
#include <unordered_map>
#include <cstddef>
#include <any>
#include <ranges>

namespace winter_rain_ecs
{
    template <typename Ev>
    class EventList
    {
    public:
        EventList(Accessor &accessor) : m_accessor(accessor) {}
        EventList(const EventList &list) = default;
        EventList(EventList &&list) = default;

        void emit(Ev event)
        {
            for (const auto &subscriber : subscribers | std::views::values)
                subscriber(m_accessor.get_world(), event);
        }

        void subscribe(std::function<void(World &, Ev)> subscriber)
        {
            auto hash_code = subscriber.target_type().hash_code();
            subscribers.insert({hash_code, subscriber});
        }

        void unsubscribe(const std::function<void(World &, Ev)> &subscriber)
        {
            auto hash_code = subscriber.target_type().hash_code();
            subscribers.erase(hash_code);
        }

    private:
        std::unordered_map<std::size_t, std::function<void(World &, Ev)>> subscribers{};
        Accessor &m_accessor;
    };

    // EventManager class
    class EventManager
    {
    public:
        EventManager(Accessor &accessor) : m_accessor(accessor) {}
        EventManager(const EventManager &manager) = delete;
        EventManager(EventManager &&manager) noexcept = default;

        template <typename Ev>
        void emit(Ev event)
        {
            if (const auto event_list_iter = m_event_lists.find(typeid(Ev).hash_code()); event_list_iter == m_event_lists.end())
            {
                m_event_lists[typeid(Ev).hash_code()] = std::make_any<EventList<Ev>>(m_accessor);
            }

            auto &event_list_any = m_event_lists[typeid(Ev).hash_code()];
            if (auto event_list = std::any_cast<EventList<Ev>>(&event_list_any))
            {
                event_list->emit(std::move(event));
            }
        }

        template <typename Ev>
        void subscribe(std::function<void(World &, Ev)> subscriber)
        {
            if (const auto event_list_iter = m_event_lists.find(typeid(Ev).hash_code()); event_list_iter == m_event_lists.end())
            {
                m_event_lists[typeid(Ev).hash_code()] = std::make_any<EventList<Ev>>(m_accessor);
            }

            auto &event_list_any = m_event_lists[typeid(Ev).hash_code()];
            if (auto event_list = std::any_cast<EventList<Ev>>(&event_list_any))
            {
                event_list->subscribe(std::move(subscriber));
            }
        }

        template <typename Ev>
        void unsubscribe(const std::function<void(World &, Ev)> &subscriber)
        {
            if (const auto event_list_iter = m_event_lists.find(typeid(Ev).hash_code()); event_list_iter != m_event_lists.end())
            {
                auto &event_list_any = m_event_lists[typeid(Ev).hash_code()];
                if (auto event_list = std::any_cast<EventList<Ev>>(&event_list_any))
                {
                    event_list->unsubscribe(subscriber);
                }
            }
        }

    private:
        std::unordered_map<std::size_t, std::any> m_event_lists{};
        Accessor &m_accessor;
    };
}
