#pragma once

#include "base/exception.h"

namespace gorc {
namespace cog {
namespace verbs {

class verb_exception : public exception { };
class undefined_verb_exception : public verb_exception { };
class verb_redefinition_exception : public verb_exception { };

}
}
}
