#include "default_verbs.hpp"
#include "continuation.hpp"
#include "executor.hpp"
#include "restart_exception.hpp"
#include "suspend_exception.hpp"
#include "sleep_record.hpp"
#include "utility/enum_hash.hpp"
#include <unordered_map>
#include <random>

using namespace gorc;
using namespace gorc::cog;

namespace {

    std::default_random_engine rng((std::random_device())());

    std::unordered_map<gorc::cog::value_type,
                       int,
                       gorc::enum_hash<gorc::cog::value_type>> value_type_message_type_map {
        { gorc::cog::value_type::nothing, 0 },
        { gorc::cog::value_type::sector, 5 },
        { gorc::cog::value_type::surface, 6 },
        { gorc::cog::value_type::thing, 3 },
        { gorc::cog::value_type::cog, 9 }
    };

    int value_type_to_message_thing_type(gorc::cog::value_type vt)
    {
        auto it = value_type_message_type_map.find(vt);
        if(it == value_type_message_type_map.end()) {
            LOG_WARNING(gorc::format("extracted type number from unmapped type %s") %
                        gorc::cog::as_string(vt));
            return 1;
        }

        return it->second;
    }

    void add_system_verbs(verb_table &verbs)
    {
        class returnex_verb : public cog::verb {
        public:
            returnex_verb(std::string const &name)
                : verb(name,
                       value_type::dynamic,
                       std::vector<value_type> { value_type::dynamic })
            {
                return;
            }

            virtual cog::value invoke(cog::stack &stk, service_registry &sr, bool) const override
            {
                auto &cc = sr.get<continuation>();
                cc.frame().return_register = stk.back();
                stk.pop_back();

                return value();
            }
        };

        verbs.emplace_verb<returnex_verb>("returnex");

        class getparam_verb : public cog::verb {
        public:
            getparam_verb(std::string const &name)
                : verb(name,
                       value_type::dynamic,
                       std::vector<value_type> { value_type::integer })
            {
                return;
            }

            virtual cog::value invoke(cog::stack &stk, service_registry &sr, bool) const override
            {
                auto &cc = sr.get<continuation>();
                int param_num = stk.back();
                stk.pop_back();

                switch(param_num) {
                case 0:
                    return cc.frame().param0;
                case 1:
                    return cc.frame().param1;
                case 2:
                    return cc.frame().param2;
                case 3:
                    return cc.frame().param3;
                default:
                    LOG_ERROR(format("getparam number %d is out of bounds") % param_num);
                    return value();
                }
            }
        };

        verbs.emplace_verb<getparam_verb>("getparam");

        class getsenderid_verb : public cog::verb {
        public:
            getsenderid_verb(std::string const &name)
                : verb(name,
                       value_type::dynamic,
                       std::vector<value_type> { })
            {
                return;
            }

            virtual cog::value invoke(cog::stack &, service_registry &sr, bool) const override
            {
                return sr.get<continuation>().frame().sender_id;
            }
        };

        verbs.emplace_verb<getsenderid_verb>("getsenderid");

        class getsenderref_verb : public cog::verb {
        public:
            getsenderref_verb(std::string const &name)
                : verb(name,
                       value_type::dynamic,
                       std::vector<value_type> { })
            {
                return;
            }

            virtual cog::value invoke(cog::stack &, service_registry &sr, bool) const override
            {
                return sr.get<continuation>().frame().sender;
            }
        };

        verbs.emplace_verb<getsenderref_verb>("getsenderref");

        class getsendertype_verb : public cog::verb {
        public:
            getsendertype_verb(std::string const &name)
                : verb(name,
                       value_type::integer,
                       std::vector<value_type> { })
            {
                return;
            }

            virtual cog::value invoke(cog::stack &, service_registry &sr, bool) const override
            {
                return value_type_to_message_thing_type(
                    sr.get<continuation>().frame().sender.get_type());
            }
        };

        verbs.emplace_verb<getsendertype_verb>("getsendertype");

        class getsourceref_verb : public cog::verb {
        public:
            getsourceref_verb(std::string const &name)
                : verb(name,
                       value_type::dynamic,
                       std::vector<value_type> { })
            {
                return;
            }

            virtual cog::value invoke(cog::stack &, service_registry &sr, bool) const override
            {
                return sr.get<continuation>().frame().source;
            }
        };

        verbs.emplace_verb<getsourceref_verb>("getsourceref");

        class getsourcetype_verb : public cog::verb {
        public:
            getsourcetype_verb(std::string const &name)
                : verb(name,
                       value_type::integer,
                       std::vector<value_type> { })
            {
                return;
            }

            virtual cog::value invoke(cog::stack &, service_registry &sr, bool) const override
            {
                return value_type_to_message_thing_type(
                    sr.get<continuation>().frame().source.get_type());
            }
        };

        verbs.emplace_verb<getsourcetype_verb>("getsourcetype");

        verbs.add_service_verb("sleep", [](bool expects_value,
                                           service_registry const &sr,
                                           float time) {
                auto &cc = sr.get<continuation>();
                auto &exec = sr.get<executor>();

                if(expects_value) {
                    // The void return value from this verb is used.
                    // Poorly written script, but the missing value will cause problems.
                    cc.data_stack.push_back(value());
                }

                exec.add_sleep_record(std::make_unique<sleep_record>(continuation(cc),
                                                                     time_delta(time)));
                throw suspend_exception();
            });

        class sendmessageex_verb : public cog::verb {
        public:
            sendmessageex_verb(std::string const &name)
                : verb(name,
                       value_type::dynamic,
                       std::vector<value_type> { value_type::cog,
                                                 value_type::message,
                                                 value_type::dynamic,
                                                 value_type::dynamic,
                                                 value_type::dynamic,
                                                 value_type::dynamic })
            {
                return;
            }

            virtual cog::value invoke(cog::stack &stk, service_registry &sr, bool ev) const override
            {
                value param3 = stk.back();
                stk.pop_back();

                value param2 = stk.back();
                stk.pop_back();

                value param1 = stk.back();
                stk.pop_back();

                value param0 = stk.back();
                stk.pop_back();

                message_id mid = stk.back();
                message_type msg = static_cast<message_type>(static_cast<int>(mid));
                stk.pop_back();

                cog_id cog = stk.back();
                stk.pop_back();

                auto &cc = sr.get<continuation>();
                auto &exec = sr.get<executor>();

                auto frame = exec.create_message_frame(cog,
                                                       msg,
                                                       /* sender */ cc.frame().instance_id,
                                                       /* TODO: senderid */ value(),
                                                       /* TODO: source */ value(),
                                                       param0,
                                                       param1,
                                                       param2,
                                                       param3);
                if(!frame.has_value()) {
                    // Warning has already been logged
                    return value();
                }

                auto &real_frame = frame.get_value();
                real_frame.save_return_register = false;
                real_frame.push_return_register = ev;

                cc.call_stack.push_back(real_frame);

                throw restart_exception();
            }
        };

        verbs.emplace_verb<sendmessageex_verb>("sendmessageex");

        class sendmessage_verb : public cog::verb {
        public:
            sendmessage_verb(std::string const &name)
                : verb(name,
                       value_type::dynamic,
                       std::vector<value_type> { value_type::cog,
                                                 value_type::message })
            {
                return;
            }

            virtual cog::value invoke(cog::stack &stk, service_registry &sr, bool ev) const override
            {
                message_id mid = stk.back();
                message_type msg = static_cast<message_type>(static_cast<int>(mid));
                stk.pop_back();

                cog_id cog = stk.back();
                stk.pop_back();

                auto &cc = sr.get<continuation>();
                auto &exec = sr.get<executor>();

                auto frame = exec.create_message_frame(cog,
                                                       msg,
                                                       /* sender */ cc.frame().instance_id,
                                                       /* TODO: senderid */ value(),
                                                       /* TODO: source */ value(),
                                                       value(),
                                                       value(),
                                                       value(),
                                                       value());
                if(!frame.has_value()) {
                    // Warning has already been logged
                    return value();
                }

                auto &real_frame = frame.get_value();
                real_frame.save_return_register = false;
                real_frame.push_return_register = ev;

                cc.call_stack.push_back(real_frame);

                throw restart_exception();
            }
        };

        verbs.emplace_verb<sendmessage_verb>("sendmessage");

        return;
    }

    void add_math_verbs(verb_table &verbs)
    {
        verbs.add_verb("bitclear", [](int flag, int clear_flags) {
                return flag & ~clear_flags;
            });

        verbs.add_verb("bitset", [](int flag, int set_flags) {
                return flag | set_flags;
            });

        verbs.add_verb("bittest", [](int a, int b) {
                return a & b;
            });

        verbs.add_verb("rand", []() {
                return std::generate_canonical<float, 10>(rng);
            });

        verbs.add_verb("randvec", []() {
                auto vec = make_vector(std::generate_canonical<float, 10>(rng),
                                       std::generate_canonical<float, 10>(rng),
                                       std::generate_canonical<float, 10>(rng));
                vec *= 2.0f;
                vec -= make_vector(1.0f, 1.0f, 1.0f);
                return vec;
            });

        verbs.add_verb("vectorx", [](vector<3> a) {
                return get<0>(a);
            });

        verbs.add_verb("vectory", [](vector<3> a) {
                return get<1>(a);
            });

        verbs.add_verb("vectorz", [](vector<3> a) {
                return get<2>(a);
            });

        verbs.add_verb("vectorset", [](float x, float y, float z) {
                return make_vector(x, y, z);
            });

        verbs.add_verb("vectoradd", [](vector<3> a, vector<3> b) {
                return a + b;
            });

        verbs.add_verb("vectorsub", [](vector<3> a, vector<3> b) {
                return a - b;
            });

        verbs.add_verb("vectorscale", [](vector<3> a, float c) {
                return a * c;
            });

        verbs.add_verb("vectorlen", [](vector<3> a) {
                return length(a);
            });

        verbs.add_verb("vectordist", [](vector<3> a, vector<3> b) {
                return length(a - b);
            });

        verbs.add_verb("vectornorm", [](vector<3> a) {
                return normalize(a);
            });

        verbs.add_verb("vectordot", [](vector<3> a, vector<3> b) {
                return dot(a, b);
            });

        verbs.add_verb("vectorcross", [](vector<3> a, vector<3> b) {
                return cross(a, b);
            });
    }
}

void gorc::cog::default_populate_verb_table(verb_table &verbs)
{
    add_system_verbs(verbs);
    add_math_verbs(verbs);
}
