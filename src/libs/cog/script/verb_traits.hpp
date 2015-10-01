#pragma once

#include "value.hpp"
#include "utility/make_fake.hpp"
#include <type_traits>
#include <vector>

namespace gorc {
    namespace cog {

        namespace detail {
            template <typename ValueT>
            struct get_value_type;

            template <>
            struct get_value_type<void> {
                static constexpr auto value = value_type::nothing;
            };

            template <>
            struct get_value_type<char const *> {
                static constexpr auto value = value_type::string;
            };

            template <>
            struct get_value_type<bool> {
                static constexpr auto value = value_type::boolean;
            };

            template <>
            struct get_value_type<int> {
                static constexpr auto value = value_type::integer;
            };

            template <>
            struct get_value_type<float> {
                static constexpr auto value = value_type::floating;
            };

            template <>
            struct get_value_type<vector<3>> {
                static constexpr auto value = value_type::vector;
            };

#define MAKE_ID_VALUE_TYPE(x) \
            template <> \
            struct get_value_type<x##_id> { \
                static constexpr auto value = value_type::x; \
            }

            MAKE_ID_VALUE_TYPE(sector);
            MAKE_ID_VALUE_TYPE(surface);
            MAKE_ID_VALUE_TYPE(thing);
            MAKE_ID_VALUE_TYPE(player);
            MAKE_ID_VALUE_TYPE(ai);
            MAKE_ID_VALUE_TYPE(cog);
            MAKE_ID_VALUE_TYPE(keyframe);
            MAKE_ID_VALUE_TYPE(material);
            MAKE_ID_VALUE_TYPE(model);
            MAKE_ID_VALUE_TYPE(sound);
            MAKE_ID_VALUE_TYPE(thing_template);

#undef MAKE_ID_VALUE_TYPE
        }

        template <typename ValueT>
        struct get_value_type : public detail::get_value_type<typename std::decay<ValueT>::type> { };

        namespace detail {
            class anything {
            public:
                template <typename T>
                operator T() const;

                template <typename T>
                operator T&();
            };

            template <typename FnT, typename ...ArgT>
            constexpr auto compute_verb_arity(FnT const &fn, int)
                -> typename std::enable_if<std::is_same<decltype(fn(ArgT()...)),
                                                        decltype(fn(ArgT()...))>::value,
                                           size_t>::type
            {
                return sizeof...(ArgT);
            }

            template <typename FnT, typename ...ArgT>
            constexpr size_t compute_verb_arity(FnT const &fn, ...)
            {
                return compute_verb_arity<FnT, ArgT..., detail::anything>(fn, 0);
            }
        }

        template <typename FnT>
        constexpr size_t compute_verb_arity(FnT const &fn)
        {
            return detail::compute_verb_arity(fn, 0);
        }

        namespace detail {
            template <size_t n, typename ...ArgT>
            struct compute_verb_result_type
                : public compute_verb_result_type<n - 1, ArgT..., anything> { };

            template <typename ...ArgT>
            struct compute_verb_result_type<0, ArgT...> {
                template <typename FnT>
                auto operator()(FnT const &fn)
                    -> decltype(fn(ArgT()...)) { }
            };
        }

        template <typename FnT>
        constexpr value_type compute_verb_result_type(FnT const &fn)
        {
            return get_value_type<decltype(
                    detail::compute_verb_result_type<compute_verb_arity(fn)>()(fn))>::value;
        }

        namespace detail {
            template <size_t pre_n, size_t post_n, typename InsertT, typename ...ArgT>
            struct apply_pre_verb_argument_type
                : public apply_pre_verb_argument_type<pre_n - 1,
                                                      post_n,
                                                      InsertT,
                                                      ArgT...,
                                                      anything> { };

            template <size_t post_n, typename InsertT, typename ...ArgT>
            struct apply_post_verb_argument_type
                : public apply_post_verb_argument_type<post_n - 1,
                                                       InsertT,
                                                       ArgT...,
                                                       anything> { };

            template <size_t post_n, typename InsertT, typename ...ArgT>
            struct apply_pre_verb_argument_type<0, post_n, InsertT, ArgT...>
                : public apply_post_verb_argument_type<post_n,
                                                       InsertT,
                                                       ArgT...,
                                                       InsertT> { };

            template <typename InsertT, typename ...ArgT>
            struct apply_post_verb_argument_type<0, InsertT, ArgT...> {
                template <typename FnT>
                static auto get(FnT const &fn) -> decltype(fn(make_fake<ArgT>()...));
            };

            template <size_t n, size_t arity, typename InsertT>
            struct apply_verb_argument_type
                : public apply_pre_verb_argument_type<n,
                                                      arity - n - 1,
                                                      InsertT> { };

            template <size_t n, size_t arity, typename InsertT, typename FnT>
            auto enable_if_applied(FnT const &fn)
                -> typename std::enable_if<std::is_same<
                        decltype(apply_verb_argument_type<n, arity, anything>::get(fn)),
                        decltype(apply_verb_argument_type<n, arity, InsertT>::get(fn))>::value,
                        value_type>;

#define MAKE_VERB_ARGUMENT_TYPE(x, y) \
            template <size_t n, size_t arity, typename FnT> \
            constexpr auto compute_verb_argument_type(FnT const &fn) \
                -> typename decltype(enable_if_applied<n, arity, x>(fn))::type \
            { \
                return y; \
            }

            MAKE_VERB_ARGUMENT_TYPE(float, value_type::floating)
            MAKE_VERB_ARGUMENT_TYPE(char const *, value_type::string)
            MAKE_VERB_ARGUMENT_TYPE(vector<3>, value_type::vector)

#define MAKE_VERB_ARGUMENT_ID_TYPE(x) \
            MAKE_VERB_ARGUMENT_TYPE(x##_id, value_type::x)

            MAKE_VERB_ARGUMENT_ID_TYPE(sector)
            MAKE_VERB_ARGUMENT_ID_TYPE(surface)
            MAKE_VERB_ARGUMENT_ID_TYPE(thing)
            MAKE_VERB_ARGUMENT_ID_TYPE(player)
            MAKE_VERB_ARGUMENT_ID_TYPE(ai)
            MAKE_VERB_ARGUMENT_ID_TYPE(cog)
            MAKE_VERB_ARGUMENT_ID_TYPE(keyframe)
            MAKE_VERB_ARGUMENT_ID_TYPE(material)
            MAKE_VERB_ARGUMENT_ID_TYPE(model)
            MAKE_VERB_ARGUMENT_ID_TYPE(sound)
            MAKE_VERB_ARGUMENT_ID_TYPE(thing_template)

#undef MAKE_VERB_ARGUMENT_ID_TYPE
#undef MAKE_VERB_ARGUMENT_TYPE
        }

        template <size_t n, typename FnT>
        constexpr value_type compute_verb_argument_type(FnT const &fn)
        {
            static_assert(n < compute_verb_arity(fn), "argument index out of bounds");
            return detail::compute_verb_argument_type<n, compute_verb_arity(fn)>(fn);
        }

        namespace detail {
            template <size_t curr, size_t max>
            struct extract_verb_argument_types {
                template <typename FnT, typename InsertIt>
                void operator()(FnT const &fn, InsertIt it)
                {
                    (*it) = cog::compute_verb_argument_type<curr>(fn);
                    ++it;
                    extract_verb_argument_types<curr + 1, max>()(fn, it);
                }
            };

            template <size_t max>
            struct extract_verb_argument_types<max, max> {
                template <typename FnT, typename InsertIt>
                void operator()(FnT const &, InsertIt)
                {
                    return;
                }
            };
        }

        template <typename FnT, typename InsertIt>
        void compute_verb_argument_types(FnT const &fn, InsertIt it)
        {
            detail::extract_verb_argument_types<0, compute_verb_arity(fn)>()(fn, it);
        }

        template <typename FnT>
        std::vector<value_type> compute_verb_argument_types(FnT const &fn)
        {
            std::vector<value_type> rv;
            compute_verb_argument_types(fn, std::back_inserter(rv));
            return rv;
        }

        template <typename FnT, typename InsertIt>
        void compute_service_verb_argument_types(FnT const &fn, InsertIt it)
        {
            detail::extract_verb_argument_types<1, compute_verb_arity(fn)>()(fn, it);
        }

        template <typename FnT>
        std::vector<value_type> compute_service_verb_argument_types(FnT const &fn)
        {
            std::vector<value_type> rv;
            compute_service_verb_argument_types(fn, std::back_inserter(rv));
            return rv;
        }
    }
}
