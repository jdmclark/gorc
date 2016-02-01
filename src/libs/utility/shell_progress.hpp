#pragma once

#include "progress.hpp"
#include <memory>
#include <cstddef>
#include <string>
#include <iostream>

namespace gorc {

    class shell_progress : public progress {
    private:
        std::ostream &stream;
        size_t steps;
        size_t width;
        size_t current = 0;
        size_t advance_calls = 0;

        void render(bool draw_spinner);

    public:
        shell_progress(size_t steps, size_t width = 80, std::ostream &stream = std::cerr);
        virtual void advance(size_t steps_to_advance = 1) override;
        virtual void finished() override;
    };

    class shell_progress_factory : public progress_factory {
    private:
        std::ostream &stream;
        size_t width;

    public:
        shell_progress_factory(size_t width = 80, std::ostream &stream = std::cerr);
        virtual std::unique_ptr<progress> make_progress(size_t steps) override;
    };

}
