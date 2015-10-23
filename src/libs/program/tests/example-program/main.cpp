#include "program/program.hpp"
#include <iostream>

using namespace gorc;

class test_program : public program {
public:
    std::string message;
    std::string info_message;
    std::string warning_message;
    std::string error_message;
    std::string fatal_message;
    bool throw_std_exception = false;
    bool throw_unk_exception = false;
    bool long_message = false;

    virtual void create_options(options &opts) override
    {
        opts.insert(make_value_option("message", message));
        opts.insert(make_value_option("info-message", info_message));
        opts.insert(make_value_option("warning-message", warning_message));
        opts.insert(make_value_option("error-message", error_message));
        opts.insert(make_value_option("fatal-message", fatal_message));
        opts.insert(make_switch_option("throw-std-exception", throw_std_exception));
        opts.insert(make_switch_option("throw-unk-exception", throw_unk_exception));
        opts.insert(make_switch_option("long-message", long_message));
    }

    virtual int main() override
    {
        LOG_TRACE("test_program::main");

        LOG_DEBUG("handling message options");

        if(!message.empty()) {
            std::cout << message << std::endl;
        }

        if(!info_message.empty()) {
            LOG_INFO(info_message);
        }

        if(!warning_message.empty()) {
            LOG_WARNING(warning_message);
        }

        if(!error_message.empty()) {
            LOG_ERROR(error_message);
        }

        if(!fatal_message.empty()) {
            LOG_FATAL(fatal_message);
        }

        if(long_message) {
            LOG_WARNING("very long message sent over the network "
                        "very long message sent over the network "
                        "very long message sent over the network "
                        "very long message sent over the network "
                        "very long message sent over the network "
                        "very long message sent over the network "
                        "very long message sent over the network "
                        "very long message sent over the network "
                        "very long message sent over the network "
                        "very long message sent over the network");
        }

        LOG_DEBUG("handling exception options");

        if(throw_std_exception) {
            throw std::runtime_error("standard exception thrown");
        }

        if(throw_unk_exception) {
            throw std::string("unknown exception thrown");
        }

        return EXIT_SUCCESS;
    }
};

MAKE_MAIN(test_program)
