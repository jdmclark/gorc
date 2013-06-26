#include "episode.h"
#include "exception.h"
#include "framework/text/source.h"
#include "framework/text/tokenizer.h"
#include "framework/io/exception.h"
#include "framework/diagnostics/report.h"
#include "framework/diagnostics/helper.h"
#include <boost/algorithm/string.hpp>

Gorc::Content::VFS::Episode::Episode::Episode(IO::ReadOnlyFile& file, Diagnostics::Report& report) {
	Text::Source src(file);
	Text::Tokenizer tok(src, report);
	Text::Token t;

	EpisodeName = tok.GetStringLiteral();

	tok.AssertIdentifier("TYPE");
	Type = static_cast<EpisodeType>(tok.GetNumber<int>());

	tok.AssertIdentifier("SEQ");
	int nEntries = tok.GetNumber<int>();

	while(true) {
		tok.GetToken(t);
		if(t.Type == Text::TokenType::EndOfFile) {
			Diagnostics::Helper::UnexpectedEndOfFile(report, "episode", t.Location);
			return;
		}
		else if(t.Type == Text::TokenType::Identifier && t.Value == "end") {
			return;
		}
		else if(t.Type == Text::TokenType::Integer) {
			int number = t.GetNumericValue<int>();
			tok.AssertPunctuator(":");
			int disc = tok.GetNumber<int>();
			int level = tok.GetNumber<int>();

			EntryType entryType;

			std::string entryToken = tok.GetIdentifier();
			if(boost::iequals(entryToken, "CUT")) {
				entryType = EntryType::Cutscene;
			}
			else if(boost::iequals(entryToken, "LEVEL")) {
				entryType = EntryType::Level;
			}
			else if(boost::iequals(entryToken, "DECIDE")) {
				entryType = EntryType::Decision;
			}
			else {
				report.AddError("episode", "unrecognized episode entry type", tok.GetInternalTokenLocation());
				throw IO::FileCorruptException();
			}

			std::string filename = tok.GetSpaceDelimitedString();
			int lightpow = tok.GetNumber<int>();
			int darkpow = tok.GetNumber<int>();
			int gotoa = tok.GetNumber<int>();
			int gotob = tok.GetNumber<int>();

			entryMap.insert(std::make_pair(number, entries.size()));
			entries.emplace_back(number, disc, level, entryType, filename, lightpow, darkpow, gotoa, gotob);
		}
		else {
			Diagnostics::Helper::Expected(report, "episode", "entry number or end", t.Location);
			throw IO::FileCorruptException();
		}
	}

	return;
}

const Gorc::Content::VFS::Episode::Entry& Gorc::Content::VFS::Episode::Episode::GetEntry(int LineNumber) const {
	auto it = entryMap.find(LineNumber);
	if(it == entryMap.end()) {
		throw LineDoesNotExistException();
	}

	return entries[it->second];
}
