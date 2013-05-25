#pragma once

#include "framework/exception.h"

namespace Gorc {
namespace Cog {
namespace Verbs {

class VerbException : public Exception { };
class UndefinedVerbException : public VerbException { };
class VerbRedefinitionException : public VerbException { };

}
}
}
