#pragma once

#include "framework/exception.h"

namespace Gorc {
namespace Content {
namespace VFS {
namespace Episode {

class EpisodeException : public Exception { };
class LineDoesNotExistException : public Exception { };

}
}
}
}
