#pragma once
#include <unordered_map>
#include <memory>
#include <cstddef>

namespace darkriver
{
    class BaseResource
    {
    public:
        virtual ~BaseResource() = default;
    };

    template <typename T>
    class Resource : public BaseResource
    {
    private:
        T m_resource;

    public:
        Resource(T&& resource) : m_resource(resource) {}
        // access the resource without a method by operator overloading
        T &operator*() { return m_resource; }
    };

    class ResourcesManager
    {
    private:
        std::unordered_map<std::size_t, std::shared_ptr<BaseResource>> m_resources{};

    public:
        template <typename T>
        void add(T &&resource)
        {
            std::size_t id = typeid(T).hash_code();
            m_resources[id] = std::make_shared<Resource<T>>(resource);
        }

        template <typename T>
        Resource<T> &get()
        {
            std::size_t id = typeid(T).hash_code();
            return *std::dynamic_pointer_cast<Resource<T>>(m_resources[id]);
        }
    };
}