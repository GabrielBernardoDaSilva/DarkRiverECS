#pragma once
#include <unordered_map>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>

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
        std::allocator<T> m_allocator{};
        std::allocator_traits<std::allocator<T>> m_traits{};
        std::allocator<int32_t> m_int_allocator{};
        std::allocator_traits<std::allocator<int32_t>> m_int_traits{};

        T *m_resource_ptr;
        int32_t *m_ref_count_ptr;

    public:
        Resource(T &resource)
        {
            m_resource_ptr = m_allocator.allocate(1);
            m_traits.construct(m_allocator, m_resource_ptr, resource);

            m_ref_count_ptr = m_int_allocator.allocate(1);
            m_int_traits.construct(m_int_allocator, m_ref_count_ptr, 1);
        }

        Resource(T &&resource)
        {
            m_resource_ptr = m_allocator.allocate(1);
            m_traits.construct(m_allocator, m_resource_ptr, std::move(resource));

            m_ref_count_ptr = m_int_allocator.allocate(1);
            m_int_traits.construct(m_int_allocator, m_ref_count_ptr, 1);
        }

        Resource(const Resource &other)
        {
            m_resource_ptr = other.m_resource_ptr;
            m_ref_count_ptr = other.m_ref_count_ptr;
            (*m_ref_count_ptr)++;
        }

        Resource &operator=(const Resource &other)
        {
            if (this == &other)
            {
                return *this;
            }

            m_resource_ptr = other.m_resource_ptr;
            m_ref_count_ptr = other.m_ref_count_ptr;
            (*m_ref_count_ptr)++;

            return *this;
        }

        Resource(Resource &&other)
        {
            m_resource_ptr = other.m_resource_ptr;
            m_ref_count_ptr = other.m_ref_count_ptr;
            other.m_resource_ptr = nullptr;
            other.m_ref_count_ptr = nullptr;
        }

        Resource &operator=(Resource &&other)
        {
            if (this == &other)
            {
                return *this;
            }

            m_resource_ptr = other.m_resource_ptr;
            m_ref_count_ptr = other.m_ref_count_ptr;
            other.m_resource_ptr = nullptr;
            other.m_ref_count_ptr = nullptr;

            return *this;
        }

        ~Resource()
        {
            if (m_resource_ptr == nullptr || m_ref_count_ptr == nullptr)
                return;

            if (--(*m_ref_count_ptr) == 0)
            {

                m_allocator.deallocate(m_resource_ptr, 1);
                m_int_allocator.deallocate(m_ref_count_ptr, 1);
            }
        }

        // access the resource without a method by operator overloading
        T &operator*()
        {
            return *m_resource_ptr;
        }

        // Access the resource's members using the arrow operator
        T *operator->()
        {
            return m_resource_ptr;
        }

        // Access the resource's members using the arrow operator
        const T *operator->() const
        {
            return m_resource_ptr;
        }

        // Access the resource's members using the arrow operator
        const T &operator*() const
        {
            return *m_resource_ptr;
        }
    };

    class ResourceManager
    {
    private:
        std::unordered_map<std::size_t, std::shared_ptr<BaseResource>> m_resources{};

    public:
        template <typename T>
        void add(T &&resource)
        {
            std::size_t id = typeid(T).hash_code();
            m_resources[id] = std::make_shared<Resource<std::remove_reference_t<T>>>(resource);
        }

        template <typename T>
        void remove()
        {
            std::size_t id = typeid(T).hash_code();
            m_resources.erase(id);
        }

        template <typename T>
        T &get_with_resource_type()
        {
            std::size_t id = typeid(T).hash_code();
            return *std::dynamic_pointer_cast<Resource<T>>(m_resources[id]);
        }

        template <typename T>
        Resource<T> get()
        {
            std::size_t id = typeid(T).hash_code();
            if (m_resources.find(id) == m_resources.end())
            {
                throw std::invalid_argument("Resource not found");
            }
            return *std::dynamic_pointer_cast<Resource<T>>(m_resources[id]);
        }
    };
}