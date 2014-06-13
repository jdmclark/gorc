#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <boost/integer/static_log2.hpp>
#include <cassert>
#include <cstdlib>

namespace gorc {
inline namespace utility {

template <typename T> class pool_ptr {
public:
    std::unique_ptr<T> value;

    inline T& operator*() {
        return *value;
    }

    inline const T& operator*() const {
        return *value;
    }

    inline T* operator->() {
        return value.get();
    }

    inline T const* operator->() const {
        return value.get();
    }
};

template <typename T, size_t page_size = 128> class pool {
    static_assert((page_size & (page_size - 1)) == 0, "page_size must be a power of 2");
    static const int page_number_shift = boost::static_log2<page_size>::value;
    static const size_t page_offset_mask = page_size - 1;
    friend class iterator;
    friend class const_iterator;

public:
    class element : public T {
        friend class pool;
        friend class page;
        element* pool_next_free = nullptr;
        int pool_element_id = -1;
        bool pool_element_initialized = false;

    public:
        element() = default;

        inline int get_id() const {
            return pool_element_id;
        }

        using T::T;
    };

    class page {
    private:
        element* data;

    public:
        page() {
            data = reinterpret_cast<element*>(malloc(sizeof(element) * page_size));
        }

        page(page&& v) {
            data = v.data;
            v.data = nullptr;
        }

        page(const page&) = delete;

        ~page() {
            if(data) {
                for(size_t i = 0; i < page_size; ++i) {
                    element& em = data[i];
                    if(em.pool_element_initialized) {
                        em.~element();
                    }
                }

                free(data);
                data = nullptr;
            }
        }

        inline element& operator[](size_t index) {
            return data[index];
        }

        inline const element& operator[](size_t index) const {
            return data[index];
        }

        inline element* begin() {
            return data;
        }

        inline element const* begin() const {
            return data;
        }

        inline element* end() {
            return data + page_size;
        }

        inline element const* end() const {
            return data + page_size;
        }
    };

private:
    element* first_free = nullptr;
    std::vector<page> pages;

    void add_page() {
        size_t page_base = (pages.size() << page_number_shift) | page_offset_mask;

        pages.push_back(page());

        element* next_free = nullptr;
        for(size_t i = page_size; i > 0; --i) {
            element& obj = pages.back()[i - 1];
            obj.pool_element_initialized = false;
            obj.pool_next_free = next_free;
            obj.pool_element_id = static_cast<int>(page_base--);
            next_free = &obj;
        }

        first_free = next_free;
    }

    element& get_pool_object(int index) {
        unsigned int page_num = index >> page_number_shift;
        unsigned int item_num = index & page_offset_mask;

        assert(page_num < pages.size() && "page num out of bounds");
        assert(item_num < page_size && "item num out of bounds");

        return pages[page_num][item_num];
    }

    const element& get_pool_object(int index) const {
        unsigned int page_num = index >> page_number_shift;
        unsigned int item_num = index & page_offset_mask;

        return pages[page_num][item_num];
    }

    size_t get_index_upper_bound() const {
        return ((pages.size() - 1) << page_number_shift) | page_offset_mask;
    }

public:
    class iterator {
    private:
        int index;
        pool* refd_pool;

        void advance() {
            size_t upper_bound = refd_pool->get_index_upper_bound();
            ++index;
            while(index < upper_bound) {
                if(refd_pool->get_pool_object(index).pool_element_initialized) {
                    return;
                }
                ++index;
            }

            index = 0;
            refd_pool = nullptr;
        }

    public:
        iterator()
            : index(0), refd_pool(nullptr) {
            return;
        }

        iterator(int index, pool* refd_pool)
            : index(index), refd_pool(refd_pool) {
            if(refd_pool->get_pool_object(index).pool_next_free) {
                advance();
            }
            return;
        }

        bool operator==(const iterator& it) const {
            return refd_pool == it.refd_pool && index == it.index;
        }

        bool operator!=(const iterator& it) const {
            return refd_pool != it.refd_pool || index != it.index;
        }

        element& operator*() const {
            return (*refd_pool)[index];
        }

        element* operator->() const {
            return &(*refd_pool)[index];
        }

        iterator& operator++() {
            advance();
            return *this;
        }

        iterator operator++(int) {
            iterator cpy(*this);
            advance();
            return cpy;
        }
    };

    class const_iterator {
    private:
        int index;
        const pool* refd_pool;

        void advance() {
            size_t upper_bound = refd_pool->get_index_upper_bound();
            ++index;
            while(index < upper_bound) {
                if(refd_pool->get_pool_object(index).pool_element_initialized) {
                    return;
                }
                ++index;
            }

            index = 0;
            refd_pool = nullptr;
        }

    public:
        const_iterator()
            : index(0), refd_pool(nullptr) {
            return;
        }

        const_iterator(int index, const pool* refd_pool)
            : index(index), refd_pool(refd_pool) {
            if(refd_pool->get_pool_object(index).pool_next_free) {
                advance();
            }
            return;
        }

        bool operator==(const const_iterator& it) const {
            return refd_pool == it.refd_pool && index == it.index;
        }

        bool operator!=(const const_iterator& it) const {
            return refd_pool != it.refd_pool || index != it.index;
        }

        const element& operator*() const {
            return (*refd_pool)[index];
        }

        const element* operator->() const {
            return &(*refd_pool)[index];
        }

        const_iterator& operator++() {
            advance();
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator cpy(*this);
            advance();
            return cpy;
        }
    };

    pool() : first_free(nullptr) {
        add_page();
        return;
    }

    element& operator[](int index) {
        return get_pool_object(index);
    }

    const element& operator[](int index) const {
        return get_pool_object(index);
    }

    template <typename... Args> element& emplace(Args&&... args) {
        if(first_free) {
            element* obj = first_free;
            first_free = obj->pool_next_free;
            auto em_id = obj->pool_element_id;

            new (obj) element(std::forward<Args>(args)...);
            obj->pool_element_initialized = true;
            obj->pool_next_free = nullptr;
            obj->pool_element_id = em_id;

            return *obj;
        }
        else {
            add_page();
            return emplace(std::forward<Args>(args)...);
        }
    }

    void erase(element& em) {
        auto em_id = em.pool_element_id;
        em.~element();
        em.pool_next_free = first_free;
        em.pool_element_initialized = false;
        em.pool_element_id = em_id;
        first_free = &em;
    }

    void erase(int index) {
        auto& obj = get_pool_object(index);
        erase(obj);
    }

    iterator begin() {
        return iterator(0, this);
    }

    iterator end() {
        return iterator();
    }

    const_iterator begin() const {
        return const_iterator(0, this);
    }

    const_iterator end() const {
        return const_iterator();
    }

    size_t size() const {
        return pages.size() * page_size;
    }

    size_t count() const {
        size_t ct = 0;
        for(const auto& page : pages) {
            for(const auto& em : page) {
                if(em.pool_element_initialized) {
                    ++ct;
                }
            }
        }

        return ct;
    }
};

}
}
