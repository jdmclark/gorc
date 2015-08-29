#pragma once

#include "verb.hpp"
#include "verb_traits.hpp"
#include "log/log.hpp"
#include "utility/make_unique.hpp"

namespace gorc {
    namespace cog {

        namespace detail {
            template <size_t remaining, value_type result_t>
            struct apply_verb_arguments {
                template <typename FnT, typename ...ArgT>
                value operator()(std::string const &name, FnT const &fn, stack &s, ArgT ...args)
                {
                    if(s.empty()) {
                        LOG_FATAL(format("stack underflow in verb '%s'") % name);
                    }

                    auto v = s.top();
                    s.pop();
                    return apply_verb_arguments<remaining - 1, result_t>()(name, fn, s, v, args...);
                }
            };

            template <value_type result_t>
            struct apply_verb_arguments<0, result_t> {
                template <typename FnT, typename ...ArgT>
                value operator()(std::string const &, FnT const &fn, stack &, ArgT ...args)
                {
                    return fn(args...);
                }
            };

            template <>
            struct apply_verb_arguments<0, value_type::nothing> {
                template <typename FnT, typename ...ArgT>
                value operator()(std::string const &, FnT const &fn, stack &, ArgT ...args)
                {
                    fn(args...);
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

            virtual value invoke(stack &s) const override
            {
                return detail::apply_verb_arguments<arity, result_type>()(name, functor, s);
            }
        };

        template <typename FnT>
        std::unique_ptr<verb> make_function_verb(std::string const &name,
                                                 FnT functor)
        {
            return make_unique<function_verb<FnT,
                                             compute_verb_arity(functor),
                                             compute_verb_result_type(functor)>>(name, functor);
        }

    }
}
