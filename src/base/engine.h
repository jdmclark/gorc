#pragma once

#include "base/place/presenter.h"
#include "base/utility/time.h"

namespace gorc {

class engine {
public:
    static std::string enginename;

    static std::string getEngine() { 
        return enginename;
    }
};

}
