#pragma once

#include "framework/exception.h"

namespace Gorc {
namespace IO {

class IOException : public Exception { };
class FileOpenErrorException : public IOException { };
class FileCreateErrorException : public IOException { };
class FileReadErrorException : public IOException { };
class FileWriteErrorException : public IOException { };
class FileInvalidSeekException : public IOException { };

}
}
