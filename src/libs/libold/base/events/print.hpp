#pragma once

#include <cstdint>
#include <string>

namespace gorc {
namespace events {

class print {
public:
    #include "print.uid"

    print(const std::string& message);

    const std::string message;
};

}
}
