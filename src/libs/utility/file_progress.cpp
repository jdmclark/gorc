#include "file_progress.hpp"
#include <vector>

gorc::file_progress::file_progress()
{
    return;
}

void gorc::file_progress::advance(size_t)
{
    return;
}

void gorc::file_progress::finished()
{
    return;
}

std::unique_ptr<gorc::progress> gorc::file_progress_factory::make_progress(size_t)
{
    return std::make_unique<file_progress>();
}
