#include "make_progress_factory.hpp"
#include "utility/file_progress.hpp"
#include "utility/shell_progress.hpp"
#include "system/env.hpp"

std::unique_ptr<gorc::progress_factory> gorc::make_boc_progress_factory()
{
    if(gorc::has_environment_variable("GORC_NO_PROGRESS")) {
        return std::make_unique<file_progress_factory>();
    }
    else {
        return std::make_unique<shell_progress_factory>();
    }
}
