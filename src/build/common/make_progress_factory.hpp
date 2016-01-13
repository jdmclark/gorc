#pragma once

#include "utility/progress.hpp"
#include <memory>

namespace gorc {

    std::unique_ptr<progress_factory> make_boc_progress_factory();

}
