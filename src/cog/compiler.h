#pragma once

#include <unordered_map>
#include <string>

#include "cog/script.h"
#include "cog/verbs/table.h"
#include "cog/vm/value.h"
#include "cog/message_id.h"
#include "framework/io/read_only_file.h"
#include "framework/diagnostics/report.h"

namespace Gorc {
namespace Cog {

class Compiler {
private:
	void AddMessageId(const std::string& name, MessageId value);
	void PopulateTables();

	std::unordered_map<std::string, VM::Value> ConstantTable;
	std::unordered_map<std::string, MessageId> MessageTable;
	Verbs::VerbTable& VerbTable;

public:
	Compiler(Verbs::VerbTable& verbTable);

	void Compile(IO::ReadOnlyFile& file, Script& output, Diagnostics::Report& report) const;
};

}
}
