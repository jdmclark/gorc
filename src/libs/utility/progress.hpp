#pragma once

#include <cstddef>
#include <memory>

namespace gorc {

    class progress {
    public:
        virtual ~progress();

        virtual void advance(size_t steps_to_advance = 1) = 0;
        virtual void finished() = 0;
    };

    class progress_factory {
    public:
        virtual ~progress_factory();

        virtual std::unique_ptr<progress> make_progress(size_t steps) = 0;
    };

}
