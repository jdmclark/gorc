#pragma once

namespace gorc {
    class uncopyable {
    protected:
        uncopyable();

    public:
        virtual ~uncopyable();

    private:
        uncopyable(uncopyable const &) = delete;
        uncopyable(uncopyable&&) = delete;
        uncopyable& operator=(uncopyable const &) = delete;
        uncopyable& operator=(uncopyable&&) = delete;
    };
}
