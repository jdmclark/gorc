#pragma once

#include <type_traits>
#include <utility>
#include <typeindex>
#include "contains_type.hpp"
#include "lcm.hpp"

namespace gorc {

    template <typename ...EmT>
    class variant_projector;

    template <typename EmHeadT, typename EmTailHeadT, typename ...EmTailT>
    class variant_projector<EmHeadT, EmTailHeadT, EmTailT...> {
    public:
        template <typename VisitorT, typename DataT>
        auto operator()(VisitorT &v, DataT &data, std::type_index tag) const
            -> typename std::common_type<
                    decltype(v.visit(*reinterpret_cast<EmHeadT*>(&data))),
                    decltype(variant_projector<EmTailHeadT, EmTailT...>()(v, data, tag))>::type
        {
            if(tag == typeid(EmHeadT)) {
                return v.visit(*reinterpret_cast<EmHeadT*>(&data));
            }
            else {
                return variant_projector<EmTailHeadT, EmTailT...>()(v, data, tag);
            }
        }

        template <typename VisitorT, typename DataT>
        auto operator()(VisitorT const &v, DataT &data, std::type_index tag) const
            -> typename std::common_type<
                    decltype(v.visit(*reinterpret_cast<EmHeadT*>(&data))),
                    decltype(variant_projector<EmTailHeadT, EmTailT...>()(v, data, tag))>::type
        {
            if(tag == typeid(EmHeadT)) {
                return v.visit(*reinterpret_cast<EmHeadT*>(&data));
            }
            else {
                return variant_projector<EmTailHeadT, EmTailT...>()(v, data, tag);
            }
        }

        template <typename VisitorT, typename DataT>
        auto operator()(VisitorT &v, DataT const &data, std::type_index tag) const
            -> typename std::common_type<
                    decltype(v.visit(*reinterpret_cast<EmHeadT const*>(&data))),
                    decltype(variant_projector<EmTailHeadT, EmTailT...>()(v, data, tag))>::type
        {
            if(tag == typeid(EmHeadT)) {
                return v.visit(*reinterpret_cast<EmHeadT const*>(&data));
            }
            else {
                return variant_projector<EmTailHeadT, EmTailT...>()(v, data, tag);
            }
        }

        template <typename VisitorT, typename DataT>
        auto operator()(VisitorT const &v, DataT const &data, std::type_index tag) const
            -> typename std::common_type<
                    decltype(v.visit(*reinterpret_cast<EmHeadT const*>(&data))),
                    decltype(variant_projector<EmTailHeadT, EmTailT...>()(v, data, tag))>::type
        {
            if(tag == typeid(EmHeadT)) {
                return v.visit(*reinterpret_cast<EmHeadT const*>(&data));
            }
            else {
                return variant_projector<EmTailHeadT, EmTailT...>()(v, data, tag);
            }
        }
    };

    template <typename EmHeadT>
    class variant_projector<EmHeadT> {
    public:
        template <typename VisitorT, typename DataT>
        auto operator()(VisitorT &v, DataT &data, std::type_index) const
            -> decltype(v.visit(*reinterpret_cast<EmHeadT*>(&data)))
        {
            return v.visit(*reinterpret_cast<EmHeadT*>(&data));
        }

        template <typename VisitorT, typename DataT>
        auto operator()(VisitorT const &v, DataT &data, std::type_index) const
            -> decltype(v.visit(*reinterpret_cast<EmHeadT*>(&data)))
        {
            return v.visit(*reinterpret_cast<EmHeadT*>(&data));
        }

        template <typename VisitorT, typename DataT>
        auto operator()(VisitorT &v, DataT const &data, std::type_index) const
            -> decltype(v.visit(*reinterpret_cast<EmHeadT const*>(&data)))
        {
            return v.visit(*reinterpret_cast<EmHeadT const*>(&data));
        }

        template <typename VisitorT, typename DataT>
        auto operator()(VisitorT const &v, DataT const &data, std::type_index) const
            -> decltype(v.visit(*reinterpret_cast<EmHeadT const*>(&data)))
        {
            return v.visit(*reinterpret_cast<EmHeadT const*>(&data));
        }
    };

    class variant_destructor_visitor {
    public:
        template <typename T>
        void visit(T &em) const
        {
            em.~T();
        }
    };

    template <typename DataStorageT, typename ...EmT>
    class variant_data_copy_visitor {
    private:
        DataStorageT &data;
    public:
        variant_data_copy_visitor(DataStorageT &data)
            : data(data)
        {
            return;
        }

        template <typename T>
        void visit(T const &em) const
        {
            new(&data) T(em);
        }
    };

    template <typename DataStorageT, typename ...EmT>
    class variant_data_move_visitor {
    private:
        DataStorageT &data;
    public:
        variant_data_move_visitor(DataStorageT &data)
            : data(data)
        {
            return;
        }

        template <typename T, typename DecayT = typename std::decay<T>::type>
        void visit(T &&em) const
        {
            new(&data) DecayT(std::move(em));
        }
    };

    template <typename ...EmT>
    class variant {
    private:
        typename std::aligned_union<0, EmT...>::type data;
        std::type_index tag;

        template <typename T, typename DecayT = typename std::decay<T>::type>
        void set_value(T const &v)
        {
            static_assert(contains_type<DecayT, EmT...>::value, "type not accepted by variant");
            new(&data) DecayT(v);
            tag = typeid(DecayT);
        }

        template <typename T, typename DecayT = typename std::decay<T>::type>
        void set_value(T &&v)
        {
            static_assert(contains_type<DecayT, EmT...>::value, "type not accepted by variant");
            new(&data) DecayT(std::forward<T>(v));
            tag = typeid(DecayT);
        }

        void delete_value()
        {
            variant_projector<EmT...>()(variant_destructor_visitor(), data, tag);
        }

    public:
        variant(variant const &v)
            : tag(v.tag)
        {
            v.accept(variant_data_copy_visitor<decltype(data), EmT...>(data));
        }

        variant(variant &v)
            : tag(v.tag)
        {
            v.accept(variant_data_copy_visitor<decltype(data), EmT...>(data));
        }

        variant(variant &&v)
            : tag(v.tag)
        {
            v.accept(variant_data_move_visitor<decltype(data), EmT...>(data));
        }

        template <typename T>
        explicit variant(T const &v)
            : tag(typeid(T))
        {
            set_value(v);
        }

        template <typename T>
        explicit variant(T &&v)
            : tag(typeid(T))
        {
            set_value(std::forward<T>(v));
        }

        ~variant()
        {
            delete_value();
        }

        template <typename T>
        variant& operator=(T const &v)
        {
            // Prevent self-assignment
            if(reinterpret_cast<char const *>(&data) != reinterpret_cast<char const*>(&v)) {
                delete_value();
                set_value(v);
            }

            return *this;
        }

        template <typename T>
        variant& operator=(T &&v)
        {
            delete_value();
            set_value(std::forward<T>(v));

            return *this;
        }

        variant& operator=(variant const &v)
        {
            // Prevent self-assignment
            if(this != &v) {
                delete_value();
                v.accept(variant_data_copy_visitor<decltype(data), EmT...>(data));
            }

            return *this;
        }

        variant& operator=(variant &&v)
        {
            // Prevent self-assignment
            if(this != &v) {
                delete_value();
                v.accept(variant_data_move_visitor<decltype(data), EmT...>(data));
            }

            return *this;
        }

        template <typename VisitorT>
        auto accept(VisitorT &v) -> decltype(variant_projector<EmT...>()(v, data, tag))
        {
            return variant_projector<EmT...>()(v, data, tag);
        }

        template <typename VisitorT>
        auto accept(VisitorT const &v) -> decltype(variant_projector<EmT...>()(v, data, tag))
        {
            return variant_projector<EmT...>()(v, data, tag);
        }

        template <typename VisitorT>
        auto accept(VisitorT &v) const -> decltype(variant_projector<EmT...>()(v, data, tag))
        {
            return variant_projector<EmT...>()(v, data, tag);
        }

        template <typename VisitorT>
        auto accept(VisitorT const &v) const -> decltype(variant_projector<EmT...>()(v, data, tag))
        {
            return variant_projector<EmT...>()(v, data, tag);
        }
    };

}
