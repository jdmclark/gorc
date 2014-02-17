#pragma once

#include <string>

namespace gorc {
namespace events {

class print {
public:
    print(const std::string& message);

    const std::string message;

    static const std::string& get_event_type();
};

}
}
