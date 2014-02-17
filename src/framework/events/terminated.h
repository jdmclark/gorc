#pragma once

#include <string>

namespace gorc {
namespace events {

class terminated {
public:
    static const std::string& get_event_type();
};

}
}
