#include "default_verbs.hpp"
#include "continuation.hpp"
#include "executor.hpp"
#include "restart_exception.hpp"
#include "suspend_exception.hpp"
#include "sleep_record.hpp"

void gorc::cog::default_populate_verb_table(verb_table &verbs)
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
            cc.frame().return_register = stk.top();
            stk.pop();

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
            int param_num = stk.top();
            stk.pop();

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

    verbs.add_service_verb("sleep", [](bool expects_value,
                                       service_registry const &sr,
                                       float time) {
            auto &cc = sr.get<continuation>();
            auto &exec = sr.get<executor>();

            if(expects_value) {
                // The void return value from this verb is used.
                // Poorly written script, but the missing value will cause problems.
                cc.data_stack.push(value());
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
            value param3 = stk.top();
            stk.pop();

            value param2 = stk.top();
            stk.pop();

            value param1 = stk.top();
            stk.pop();

            value param0 = stk.top();
            stk.pop();

            message_id mid = stk.top();
            message_type msg = static_cast<message_type>(static_cast<int>(mid));
            stk.pop();

            cog_id cog = stk.top();
            stk.pop();

            auto &cc = sr.get<continuation>();
            auto &exec = sr.get<executor>();

            auto frame = exec.create_message_frame(cog,
                                                   msg,
                                                   /* TODO: sender */ value(),
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

            cc.call_stack.push(real_frame);

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
            message_id mid = stk.top();
            message_type msg = static_cast<message_type>(static_cast<int>(mid));
            stk.pop();

            cog_id cog = stk.top();
            stk.pop();

            auto &cc = sr.get<continuation>();
            auto &exec = sr.get<executor>();

            auto frame = exec.create_message_frame(cog,
                                                   msg,
                                                   /* TODO: sender */ value(),
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

            cc.call_stack.push(real_frame);

            throw restart_exception();
        }
    };

    verbs.emplace_verb<sendmessage_verb>("sendmessage");

    return;
}
