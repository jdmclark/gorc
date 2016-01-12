#pragma once

#include "json_input_stream.hpp"
#include "utility/oxford_join.hpp"
#include "log/log.hpp"
#include <set>
#include <string>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <memory>

namespace gorc {

    template <typename T, typename ValueT>
    std::function<void(json_input_stream&, T&)> make_json_member(ValueT T::*ptr)
    {
        return [=](json_input_stream &jis, T &obj)
        {
            obj.*ptr = json_deserialize<ValueT>(jis);
        };
    }

    template <typename CastT, typename T, typename ValueT>
    std::function<void(json_input_stream&, T&)> make_json_cast_member(ValueT T::*ptr)
    {
        return [=](json_input_stream &jis, T &obj)
        {
            obj.*ptr = json_deserialize<CastT>(jis);
        };
    }

    template <typename T>
    class json_specification {
    private:
        using member_map =
            std::unordered_map<std::string, std::function<void(json_input_stream&, T&)>>;
        member_map members;
        std::set<std::string> required_members;

    public:
        json_specification(member_map &&members,
                           std::set<std::string> &&required_members = {})
            : members(members)
            , required_members(required_members)
        {
            return;
        }

        void deserialize(json_input_stream &jis, T &obj) const
        {
            std::set<std::string> seen_members;

            json_deserialize_members(jis, [&](json_input_stream &jis, std::string const &name)
                {
                    if(seen_members.find(name) != seen_members.end()) {
                        LOG_FATAL(format("element '%s' redefinition") % name);
                    }

                    seen_members.insert(name);

                    auto it = members.find(name);
                    if(it == members.end()) {
                        LOG_FATAL(format("unknown element '%s'") % name);
                    }

                    it->second(jis, obj);
                });

            std::set<std::string> absent_members;
            for(auto const &req_member : required_members) {
                if(seen_members.find(req_member) == seen_members.end()) {
                    absent_members.insert(req_member);
                }
            }

            if(!absent_members.empty()) {
                diagnostic_context dc(jis.get_diagnostic_context());
                LOG_FATAL(format("%s must be specified") % oxford_join(absent_members));
            }
        }
    };

    template <typename SpecT, typename T>
    void json_deserialize_with_specification(json_input_stream &jis,
                                             SpecT const &spec,
                                             T &obj)
    {
        spec.deserialize(jis, obj);
    }

}
