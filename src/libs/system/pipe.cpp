#include "pipe.hpp"
#include "log/log.hpp"
#include "utility/make_unique.hpp"
#include <system_error>
#include <unistd.h>
#include <stdlib.h>

gorc::pipe_input_stream::pipe_input_stream(int fd)
    : fd(fd)
{
    return;
}

gorc::pipe_input_stream::~pipe_input_stream()
{
    if(::close(fd) != 0) {
        // LCOV_EXCL_START
        // close should not be retried, even on EINTR
        LOG_ERROR(format("pipe_input_stream: %s") % std::generic_category().message(errno));
        // LCOV_EXCL_STOP
    }
}

size_t gorc::pipe_input_stream::read_some(void *dest, size_t size)
{
    // LCOV_EXCL_START
    ssize_t rv = 0;
    do {
    // LCOV_EXCL_STOP
        rv = ::read(fd, dest, size);
    // LCOV_EXCL_START
    }
    while(rv < 0 && errno == EINTR);
    // LCOV_EXCL_STOP

    if(rv == 0) {
        is_at_end = true;
    }

    if(rv >= 0) {
        return rv;
    }
    else {
        // LCOV_EXCL_START
        // system call
        throw std::system_error(errno, std::generic_category());
        // LCOV_EXCL_STOP
    }
}

bool gorc::pipe_input_stream::at_end()
{
    return is_at_end;
}

gorc::pipe_output_stream::pipe_output_stream(int fd)
    : fd(fd)
{
    return;
}

gorc::pipe_output_stream::~pipe_output_stream()
{
    if(::close(fd) != 0) {
        // close should not be retried, even on EINTR
        // LCOV_EXCL_START
        LOG_ERROR(format("pipe_output_stream: %s") % std::generic_category().message(errno));
        // LCOV_EXCL_STOP
    }
}

size_t gorc::pipe_output_stream::write_some(void const *src, size_t size)
{
    ssize_t rv = 0;
    do { // LCOV_EXCL_LINE
        rv = ::write(fd, src, size);
    }
    while(rv < 0 && errno == EINTR);

    if(rv >= 0) {
        return rv;
    }
    else {
        throw std::system_error(errno, std::generic_category());
    }
}

gorc::pipe::pipe()
{
    int pipefd[2];
    int result = ::pipe(pipefd);

    // LCOV_EXCL_START - system call
    if(result != 0) {
        throw std::system_error(errno, std::generic_category());
    }
    // LCOV_EXCL_STOP

    input = make_unique<pipe_input_stream>(pipefd[0]);
    output = make_unique<pipe_output_stream>(pipefd[1]);
}
