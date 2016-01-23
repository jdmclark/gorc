#include "default_verbs.hpp"
#include "continuation.hpp"
#include "executor.hpp"
#include "suspend_exception.hpp"
#include "sleep_record.hpp"

void gorc::cog::default_populate_verb_table(verb_table &verbs)
{
    verbs.add_service_verb("sleep", [](service_registry const &sr,
                                       float time) {
            auto &cc = sr.get<continuation>();
            auto &exec = sr.get<executor>();

            exec.add_sleep_record(std::make_unique<sleep_record>(continuation(cc), time));
            throw suspend_exception();
        });
    return;
}
