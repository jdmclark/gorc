#pragma once

#include "verb.hpp"
#include "verb_traits.hpp"
#include "log/log.hpp"
#include <memory>

namespace gorc {
    namespace cog {

        namespace detail {
            template <size_t remaining, value_type result_t, bool pass_svc>
            struct apply_verb_arguments {
                template <typename FnT, typename ...ArgT>
                value operator()(std::string const &name,
                                 FnT const &fn,
                                 stack &s,
                                 service_registry &sr,
                                 bool ev,
                                 ArgT ...args)
                {
                    if(s.empty()) {
                        LOG_FATAL(format("stack underflow in verb '%s'") % name);
                    }

                    auto v = s.top();
                    s.pop();
                    return apply_verb_arguments<remaining - 1,
                                                result_t,
                                                pass_svc>()(name, fn, s, sr, ev, v, args...);
                }
            };

            template <bool pass_svc>
            struct maybe_apply_service_arguments {
                template <typename FnT, typename ...ArgT>
                auto operator()(FnT const &fn,
                                service_registry &,
                                bool /* expects value */,
                                ArgT ...args) -> decltype(fn(args...))
                {
                    return fn(args...);
                }
            };

            template <>
            struct maybe_apply_service_arguments<true> {
                template <typename FnT, typename ...ArgT>
                auto operator()(FnT const &fn,
                                service_registry &sr,
                                bool expects_value,
                                ArgT ...args) -> decltype(fn(expects_value, sr, args...))
                {
                    return fn(expects_value, sr, args...);
                }
            };

            template <value_type result_t, bool pass_svc>
            struct apply_verb_arguments<0, result_t, pass_svc> {
                template <typename FnT, typename ...ArgT>
                value operator()(std::string const &,
                                 FnT const &fn,
                                 stack &,
                                 service_registry &sr,
                                 bool ev,
                                 ArgT ...args)
                {
                    return maybe_apply_service_arguments<pass_svc>()(fn, sr, ev, args...);
                }
            };

            template <bool pass_svc>
            struct apply_verb_arguments<0, value_type::nothing, pass_svc> {
                template <typename FnT, typename ...ArgT>
                value operator()(std::string const &,
                                 FnT const &fn,
                                 stack &,
                                 service_registry &sr,
                                 bool ev,
                                 ArgT ...args)
                {
                    maybe_apply_service_arguments<pass_svc>()(fn, sr, ev, args...);
                    return value();
                }
            };

        }

        template <typename FnT, size_t arity, value_type result_type>
        class function_verb : public verb {
        private:
            FnT functor;

        public:
            function_verb(std::string const &name, FnT functor)
                : verb(name,
                       compute_verb_result_type(functor),
                       compute_verb_argument_types(functor))
                , functor(functor)
            {
                return;
            }

            virtual value invoke(stack &s, service_registry &sr, bool expects_value) const override
            {
                return detail::apply_verb_arguments<arity,
                                                    result_type,
                                                    false>()(name, functor, s, sr, expects_value);
            }
        };

        template <typename FnT>
        std::unique_ptr<verb> make_function_verb(std::string const &name,
                                                 FnT functor)
        {
            return std::make_unique<function_verb<FnT,
                                                  compute_verb_arity(functor),
                                                  compute_verb_result_type(functor)>>(name, functor);
        }

        template <typename FnT, size_t arity, value_type result_type>
        class service_verb : public verb {
        private:
            FnT functor;

        public:
            service_verb(std::string const &name, FnT functor)
                : verb(name,
                       compute_verb_result_type(functor),
                       compute_service_verb_argument_types(functor))
                , functor(functor)
            {
                return;
            }

            virtual value invoke(stack &s, service_registry &sr, bool expects_value) const override
            {
                return detail::apply_verb_arguments<arity,
                                                    result_type,
                                                    true>()(name, functor, s, sr, expects_value);
            }
        };

        template <typename FnT>
        std::unique_ptr<verb> make_service_verb(std::string const &name,
                                                FnT functor)
        {
            return std::make_unique<service_verb<FnT,
                                                 compute_verb_arity(functor) - 2,
                                                 compute_verb_result_type(functor)>>(name, functor);
        }

    }
}
