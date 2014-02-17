#pragma once

#include <memory>

namespace gorc {
namespace game {
namespace world {
namespace animations {

class animation {
private:
    int id = -1;

public:
    bool expired = false;

    virtual ~animation();

    inline void set_id(int id) {
        this->id = id;
    }

    inline int get_id() const {
        return id;
    }

    virtual void update(double dt) = 0;
    virtual void stop();
};

}
}
}
}
