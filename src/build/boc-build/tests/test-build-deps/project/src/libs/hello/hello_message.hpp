#pragma once

#include "utility/message.hpp"

class hello_message : public message {
public:
    virtual std::string str() const override;
};
