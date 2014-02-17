#pragma once

#include "t_uncopyable.h"

namespace test {

template <class T> class singleton : uncopyable {
private:
    static T& instance;

public:
    static T& get() {
        static T t;
        return t;
    }
};

template <class T> T& singleton<T>::instance = get();

}
