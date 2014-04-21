#pragma once

#include "base/exception.h"

namespace gorc {
namespace io {

class io_exception : public exception { };
class file_open_exception : public io_exception { };
class file_create_exception : public io_exception { };
class file_read_exception : public io_exception { };
class file_seek_exception : public io_exception { };
class file_corrupt_exception : public io_exception { };
class file_not_found_exception : public io_exception { };
class json_illegal_element_exception : public io_exception { };
class json_invalid_format_exception : public io_exception { };

}
}
