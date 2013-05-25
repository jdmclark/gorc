#pragma once

#include "framework/exception.h"

namespace Gorc {
namespace Text {

class TextException : public Exception { };
class InvalidNumericConversionException : public Exception { };
class TokenizerAssertionException : public Exception { };

}
}
