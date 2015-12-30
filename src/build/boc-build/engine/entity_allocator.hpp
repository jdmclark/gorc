#pragma once

#include "base_file_entity.hpp"
#include "utility/make_unique.hpp"
#include "log/log.hpp"
#include <type_traits>
#include <vector>
#include <map>

namespace gorc {

    class entity_allocator {
    private:
        std::vector<std::unique_ptr<entity>> entities;
        std::map<path, entity*> file_map;

        template <typename T, typename ...ArgT>
        T* inner_emplace(ArgT &&...args)
        {
            auto ptr = make_unique<T>(std::forward<ArgT>(args)...);
            T *rv = ptr.get();
            entities.push_back(std::move(ptr));
            return rv;
        }

    public:

        template <typename T, typename ...ArgT>
        typename std::enable_if<std::is_base_of<base_file_entity, T>::value, T*>::type
            emplace(path const &name, ArgT &&...args)
        {
            // Return existing file entity
            auto it = file_map.find(name);
            if(it != file_map.end()) {
                T *val = dynamic_cast<T*>(it->second);
                if(val == nullptr) {
                    LOG_FATAL(format("entity '%s' type mismatch") % it->second->name());
                }

                return val;
            }

            T *ent = inner_emplace<T, path const &, ArgT...>(name, std::forward<ArgT>(args)...);
            file_map.emplace(name, ent);
            return ent;
        }

        template <typename T>
        typename std::enable_if<std::is_base_of<base_file_entity, T>::value, T*>::type
            emplace(entity_input_stream &is)
        {
            // Always assume deserialized graph is normalized
            return inner_emplace<T>(is);
        }

        template <typename T, typename ...ArgT>
        typename std::enable_if<!std::is_base_of<base_file_entity, T>::value, T*>::type
            emplace(ArgT &&...args)
        {
            return inner_emplace<T, ArgT...>(std::forward<ArgT>(args)...);
        }

        void clear();
    };

}
