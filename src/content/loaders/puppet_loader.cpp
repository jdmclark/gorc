#include "puppet_loader.h"
#include "content/assets/puppet.h"
#include "content/manager.h"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::PuppetLoader::AssetRootPath = { "misc/pup" };

namespace Gorc {
namespace Content {
namespace Loaders {

const std::unordered_map<std::string, Flags::PuppetSubmodeType> SubmodeMap {
	{ "stand", Flags::PuppetSubmodeType::Stand },
	{ "walk", Flags::PuppetSubmodeType::Walk },
	{ "run", Flags::PuppetSubmodeType::Run },
	{ "walkback", Flags::PuppetSubmodeType::WalkBack },
	{ "strafeleft", Flags::PuppetSubmodeType::StrafeLeft },
	{ "straferight", Flags::PuppetSubmodeType::StrafeRight },
	{ "death", Flags::PuppetSubmodeType::Death },
	{ "death2", Flags::PuppetSubmodeType::Death2 },
	{ "fire", Flags::PuppetSubmodeType::Fire },
	{ "hit", Flags::PuppetSubmodeType::Hit },
	{ "rising", Flags::PuppetSubmodeType::Rising },
	{ "drop", Flags::PuppetSubmodeType::Drop },
	{ "fall", Flags::PuppetSubmodeType::Fall },
	{ "activate", Flags::PuppetSubmodeType::Activate },
	{ "crouchforward", Flags::PuppetSubmodeType::CrouchForward },
	{ "toss", Flags::PuppetSubmodeType::Toss },
	{ "place", Flags::PuppetSubmodeType::Place },
	{ "magic", Flags::PuppetSubmodeType::Magic },
	{ "fire2", Flags::PuppetSubmodeType::Fire2 },
	{ "choke", Flags::PuppetSubmodeType::Choke },
	{ "leap", Flags::PuppetSubmodeType::Leap },
	{ "turnleft", Flags::PuppetSubmodeType::TurnLeft },
	{ "turnright", Flags::PuppetSubmodeType::TurnRight },
	{ "crouchback", Flags::PuppetSubmodeType::CrouchBack },
	{ "fidget", Flags::PuppetSubmodeType::Fidget },
	{ "fidget2", Flags::PuppetSubmodeType::Fidget2 },
	{ "land", Flags::PuppetSubmodeType::Land },
	{ "magic2", Flags::PuppetSubmodeType::Magic2 },
	{ "magic3", Flags::PuppetSubmodeType::Magic3 },
	{ "windup", Flags::PuppetSubmodeType::Windup },
	{ "holster", Flags::PuppetSubmodeType::Holster },
	{ "drawfists", Flags::PuppetSubmodeType::DrawFists },
	{ "drawgun", Flags::PuppetSubmodeType::DrawGun },
	{ "drawsaber", Flags::PuppetSubmodeType::DrawSaber }
};

}
}
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::PuppetLoader::Parse(Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Assets::Puppet> pup(new Assets::Puppet());

	Text::Token t;

	Assets::PuppetMode* mode = nullptr;
	while(true) {
		tok.GetToken(t);

		if(t.Type == Text::TokenType::EndOfFile) {
			break;
		}
		else if(boost::iequals(t.Value, "mode")) {
			tok.SetReportEOL(true);

			tok.AssertPunctuator("=");
			mode = &pup->Modes[tok.GetNumber<int>()];

			tok.GetToken(t);
			if(t.Type != Text::TokenType::EndOfLine) {
				tok.AssertIdentifier("BasedOn");
				tok.AssertPunctuator("=");
				*mode = pup->Modes[tok.GetNumber<int>()];
			}

			tok.SetReportEOL(false);
		}
		else if(boost::iequals(t.Value, "joints")) {
			while(true) {
				tok.GetToken(t);

				if(t.Type == Text::TokenType::EndOfFile || boost::iequals(t.Value, "end")) {
					break;
				}
				else {
					tok.AssertPunctuator("=");
					pup->JointMap.emplace(t.GetNumericValue<int>(), tok.GetNumber<int>());
				}
			}
		}
		else if(!mode) {
			report.AddWarning("PuppetLoader::Parse", "expected \'mode\', \'joints\' or \'end\'", t.Location);
			tok.SkipToNextLine();
		}
		else {
			std::transform(t.Value.begin(), t.Value.end(), t.Value.begin(), tolower);
			auto it = SubmodeMap.find(t.Value);
			if(it == SubmodeMap.end()) {
				report.AddWarning("PuppetLoader::Parse",
						boost::str(boost::format("unknown submode %s") % t.Value), t.Location);
				tok.SkipToNextLine();
			}
			else {
				Assets::PuppetSubmode& submode = mode->Submodes[static_cast<unsigned int>(it->second)];
				submode.Animation = &manager.Load<Assets::Animation>(tok.GetSpaceDelimitedString());
				submode.Flags = FlagSet<Flags::KeyFlag>(tok.GetNumber<unsigned int>());
				submode.LoPriority = tok.GetNumber<unsigned int>();
				submode.HiPriority = tok.GetNumber<unsigned int>();
			}
		}
	}

	return std::unique_ptr<Asset>(std::move(pup));
}
