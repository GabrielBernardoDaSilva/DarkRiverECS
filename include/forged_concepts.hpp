#pragma once
#include <concepts>
#include <type_traits>
#include <print>
#include <functional>

namespace winter_rain_ecs
{
#define FUNCTION_TRAITS(func) function_traits<decltype(&func)>(), #func
#define FUNCTION_TRAITS_NAME(func) #func
    class Component;
    class Event;
    class Plugin;

    class QueryBase;
    class EntityManager;
    class EventManager;
    class TaskManager;
    class ExecutorManager;

    template <typename T>
    concept req_component = std::is_base_of<Component, T>::value;

    template <typename T>
    concept validation_query_types = std::is_reference_v<T> && std::is_base_of_v<Component, std::remove_reference_t<T>>;

    template <typename T>
    concept req_event_ty = std::is_base_of<Event, T>::value;

    template <typename T>
    concept req_plugin = std::is_base_of<Plugin, T>::value;

    template <typename T>
    concept req_system_args = requires { std::is_base_of_v<QueryBase, T> ||
                                             std::is_same_v<EntityManager, std::remove_reference_t<T>> ||
                                             std::is_same_v<EventManager, std::remove_reference_t<T>> ||
                                             std::is_same_v<TaskManager, std::remove_reference_t<T>> ||
                                             std::is_same_v<ExecutorManager, std::remove_reference_t<T>>; };

    template <typename T>
    concept function_pointer = requires { std::is_function_v<std::remove_pointer_t<std::remove_reference_t<T>>>; };

    template <typename T>
    struct function_traits;

    template <typename R, typename... Args>
    struct function_traits<R (*)(Args...)>
    {
        using return_type = R;
        using argument_types = std::tuple<Args...>;
    };

    template <typename R, typename C, typename... Args>
    struct function_traits<R (C::*)(Args...)>
    {
        using return_type = R;
        using argument_types = std::tuple<Args...>;
    };

    template <typename R, typename C, typename... Args>
    struct function_traits<R (C::*)(Args...) const>
    {
        using return_type = R;
        using argument_types = std::tuple<Args...>;
    };

    template <typename... Args>
    struct function_traits<std::function<void(Args...)>>
    {
        using return_type = void;
        using argument_types = std::tuple<Args...>;
    };

    // Helper alias templates
    template <typename T>
    using function_return_type = typename function_traits<T>::return_type;

    template <typename T>
    using function_argument_types = typename function_traits<T>::argument_types;

    template <typename Func>
    struct function_traits_helper;

    template <typename T>
    void print_type()
    {
        std::println("{}", typeid(T).name());
    }

    template <typename Tuple, std::size_t... Is>
    void print_tuple_types(std::index_sequence<Is...>)
    {
        (print_type<typename std::tuple_element<Is, Tuple>::type>(), ...);
    }

    template <typename Tuple>
    void print_tuple_types()
    {
        print_tuple_types<Tuple>(std::make_index_sequence<std::tuple_size<Tuple>::value>{});
    }

}