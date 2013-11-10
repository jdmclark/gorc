#pragma once

#include "framework/exception.h"

namespace gorc {
namespace cog {
namespace symbols {

class symbol_exception : public exception { };
class undefined_symbol_exception : public symbol_exception { };
class symbol_redefinition_exception : public symbol_exception { };

}
}
}
