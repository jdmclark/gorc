#pragma once

#include "framework/exception.h"

namespace gorc {
namespace io {

class io_exception : public exception { };
class file_open_error_exception : public io_exception { };
class file_create_error_exception : public io_exception { };
class file_read_error_exception : public io_exception { };
class file_write_error_exception : public io_exception { };
class file_invalid_seek_exception : public io_exception { };
class file_corrupt_exception : public io_exception { };
class file_not_found_exception : public io_exception { };

}
}
