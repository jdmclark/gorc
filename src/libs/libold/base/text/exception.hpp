#pragma once

#include "libold/base/exception.hpp"

namespace gorc {
namespace text {

class text_exception : public exception { };
class invalid_numeric_conversion_exception : public exception { };
class tokenizer_assertion_exception : public exception { };

}
}
