#pragma once

#include <string>

class message {
public:
    virtual ~message();
    virtual std::string str() const = 0;
};
