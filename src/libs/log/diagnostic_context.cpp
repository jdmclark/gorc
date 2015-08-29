#include "log_frontend.hpp"
#include "diagnostic_context.hpp"

gorc::diagnostic_context::diagnostic_context(diagnostic_context_location const &loc)
    : diagnostic_context(loc.filename,
                         loc.first_line,
                         loc.first_col,
                         loc.last_line,
                         loc.last_col)
{
    return;
}

gorc::diagnostic_context::diagnostic_context(maybe<char const *> filename,
                                             int first_line,
                                             int first_col,
                                             int last_line,
                                             int last_col)
{
    diagnostic_context_handle = get_local<log_frontend>()->push_diagnostic_context(filename,
                                                                                   first_line,
                                                                                   first_col,
                                                                                   last_line,
                                                                                   last_col);
}

gorc::diagnostic_context::~diagnostic_context()
{
    get_local<log_frontend>()->release_diagnostic_context(diagnostic_context_handle);
}
