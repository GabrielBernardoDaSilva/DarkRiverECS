#pragma once
#include <concepts>
#include <type_traits>
#include <iostream>
#include <functional>

namespace forged_in_lost_lands_ecs
{
#define FUNCTION_TRAITS(func) function_traits<decltype(&func)>(), #func
#define FUNCTION_TRAITS_NAME(func) #func
    class Component;
    class Event;
    class Plugin;

    template <typename T>
    concept req_component = std::is_base_of<Component, T>::value;

    template <typename T>
    concept validation_query_types = std::is_reference_v<T> && std::is_base_of_v<Component, std::remove_reference_t<T>>;

    template <typename T>
    concept req_event_ty = std::is_base_of<Event, T>::value;

    template <typename T>
    concept req_plugin = std::is_base_of<Plugin, T>::value;

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
        std::cout << typeid(T).name() << std::endl;
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