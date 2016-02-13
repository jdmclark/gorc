#pragma once

#include <vector>
#include <array>
#include <memory>
#include <type_traits>

namespace gorc {

    template <typename EmT, size_t page_size = 128>
    class pool {
    private:
        using EmStorageT = typename std::aligned_storage<sizeof(EmT), alignof(EmT)>::type;
        using EmStoragePageT = std::array<EmStorageT, page_size>;
        std::vector<std::unique_ptr<EmStoragePageT>> pages;
        std::vector<EmStorageT*> free_list;

        void add_page()
        {
            free_list.reserve(free_list.size() + page_size);
            pages.push_back(std::make_unique<EmStoragePageT>());
            for(auto &em : *pages.back()) {
                free_list.push_back(&em);
            }
        }

        EmStorageT* get_storage()
        {
            if(free_list.empty()) {
                add_page();
            }

            EmStorageT *storage = free_list.back();
            free_list.pop_back();

            return storage;
        }

    public:
        template <typename ...ArgT>
        EmT& emplace(ArgT &&...args)
        {
            EmStorageT *storage = get_storage();
            new(storage) EmT(std::forward<ArgT>(args)...);

            return *reinterpret_cast<EmT*>(storage);
        }

        void erase(EmT &em)
        {
            em.~EmT();
            free_list.push_back(reinterpret_cast<EmStorageT*>(&em));
        }
    };

}
