#pragma once

#include "progress.hpp"
#include <cstddef>
#include <string>
#include <iostream>

namespace gorc {

    class file_progress : public progress {
    public:
        file_progress();
        virtual void advance(size_t steps_to_advance = 1) override;
        virtual void finished() override;
    };

    class file_progress_factory : public progress_factory {
    public:
        virtual std::unique_ptr<progress> make_progress(size_t steps) override;
    };

}
