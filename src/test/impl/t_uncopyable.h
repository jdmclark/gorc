#pragma once

namespace test {

class uncopyable {
protected:
    uncopyable() {
        return;
    }

    virtual ~uncopyable() {
        return;
    }

private:
    uncopyable(const uncopyable&);
    const uncopyable& operator=(const uncopyable&);
};

}
