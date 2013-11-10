#pragma once

#include "framework/exception.h"

namespace gorc {
namespace content {
namespace vfs {
namespace episode {

class episode_exception : public exception { };
class line_does_not_exist_exception : public exception { };

}
}
}
}
