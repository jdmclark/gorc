#pragma once

#include "framework/exception.h"

namespace gorc {
namespace content {
namespace vfs {

class vfs_exception : public exception { };
class container_file_corrupt_exception : public vfs_exception { };

}
}
}
