#pragma once

#include "framework/exception.h"

namespace Gorc {
namespace Content {
namespace VFS {

class VfsException : public Exception { };
class ContainerFileCorruptException : public VfsException { };

}
}
}
