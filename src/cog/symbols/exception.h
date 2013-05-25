#pragma once

#include "framework/exception.h"

namespace Gorc {
namespace Cog {
namespace Symbols {

class SymbolException : public Exception { };
class UndefinedSymbolException : public SymbolException { };
class SymbolRedefinitionException : public SymbolException { };

}
}
}
