#pragma once

#include "base/io/binary_output_stream.h"
#include "base/utility/make_unique.h"
#include <functional>
#include <unordered_map>
#include <memory>

namespace gorc {
inline namespace utility {

class base_component_type_serializer {
public:
    virtual ~base_component_type_serializer();
};

template <typename CompT>
class component_type_serializer : public base_component_type_serializer {
public:
    virtual ~component_type_serializer() {
        return;
    }

    virtual void serialize(CompT const &, io::binary_output_stream &) = 0;
};

class component_serializer {
private:
    template <typename CompT>
    class component_type_serializer_impl : public component_type_serializer<CompT> {
    private:
        std::function<void(CompT const &, io::binary_output_stream &)> s_fn;

    public:
        component_type_serializer_impl(std::function<void(CompT const &, io::binary_output_stream &)> s_fn)
            : s_fn(s_fn) {
            return;
        }

        virtual void serialize(CompT const &c, io::binary_output_stream &os) override {
            s_fn(c, os);
        }
    };

    std::unordered_map<uint32_t, std::unique_ptr<base_component_type_serializer>> serializers;

public:
    template <typename CompT>
    void add_serializer(std::function<void(CompT const &, io::binary_output_stream &)> s_fn) {
        serializers.emplace(static_cast<uint32_t>(CompT::UID::UID),
                            make_unique<component_type_serializer_impl<CompT>>(s_fn));
    }

    template <typename CompT>
    void add_default_serializer() {
        add_serializer<CompT>([](CompT const &comp, io::binary_output_stream &os) {
            comp.serialize(os);
        });
    }

    template <typename CompT>
    component_type_serializer<CompT>& get_serializer() {
        auto it = serializers.find(static_cast<uint32_t>(CompT::UID::UID));
        if(it != serializers.end()) {
            return dynamic_cast<component_type_serializer<CompT>&>(*it->second);
        }

        throw std::exception();
    }
};

}
}
