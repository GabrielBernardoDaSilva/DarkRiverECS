#pragma once
#include "forged_concepts.hpp"

#include <functional>
#include <unordered_map>
#include <cstddef>
#include <any>

namespace winter_rain_ecs
{
    class Event
    {
    };

    template <class Ev>
        requires req_event_ty<Ev>
    class EventList
    {
    public:
        EventList() = default;
        EventList(const EventList &list) = default;
        EventList(EventList &&list) = default;

        void emit(Ev event)
        {
            for (const auto &[_, subscriber] : subscribers)
                subscriber(event);
        }

        void subscribe(std::function<void(Ev)> subscriber)
        {
            auto hash_code = subscriber.target_type().hash_code();
            subscribers.insert({hash_code, subscriber});
        }

        void unsubscribe(const std::function<void(Ev)> &subscriber)
        {
            auto hash_code = subscriber.target_type().hash_code();
            subscribers.erase(hash_code);
        }

    private:
        std::unordered_map<std::size_t, std::function<void(Ev)>> subscribers{};
    };

    // EventManager class
    class EventManager
    {
    public:
        EventManager() = default;
        EventManager(const EventManager &manager) = delete;
        EventManager(EventManager &&manager) noexcept = default;

        template <req_event_ty Ev>
        void emit(Ev event)
        {
            if (const auto event_list_iter = event_lists.find(typeid(Ev).hash_code()); event_list_iter == event_lists.end())
            {
                event_lists[typeid(Ev).hash_code()] = std::make_any<EventList<Ev>>();
            }

            auto &event_list_any = event_lists[typeid(Ev).hash_code()];
            if (auto event_list = std::any_cast<EventList<Ev>>(&event_list_any))
            {
                event_list->emit(std::move(event));
            }
        }

        template <req_event_ty Ev>
        void subscribe(std::function<void(Ev)> subscriber)
        {
            if (const auto event_list_iter = event_lists.find(typeid(Ev).hash_code()); event_list_iter == event_lists.end())
            {
                event_lists[typeid(Ev).hash_code()] = std::make_any<EventList<Ev>>();
            }

            auto &event_list_any = event_lists[typeid(Ev).hash_code()];
            if (auto event_list = std::any_cast<EventList<Ev>>(&event_list_any))
            {
                event_list->subscribe(std::move(subscriber));
            }
        }

        template <req_event_ty Ev>
        void unsubscribe(const std::function<void(Ev)> &subscriber)
        {
            if (const auto event_list_iter = event_lists.find(typeid(Ev).hash_code()); event_list_iter != event_lists.end())
            {
                auto &event_list_any = event_lists[typeid(Ev).hash_code()];
                if (auto event_list = std::any_cast<EventList<Ev>>(&event_list_any))
                {
                    event_list->unsubscribe(subscriber);
                }
            }
        }

    private:
        std::unordered_map<std::size_t, std::any> event_lists{};
    };
}
