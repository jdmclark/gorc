#include "puppet_loader.h"
#include "content/assets/puppet.h"
#include "framework/content/manager.h"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>

const std::vector<boost::filesystem::path> gorc::content::loaders::puppet_loader::asset_root_path = { "misc/pup" };

namespace gorc {
namespace content {
namespace loaders {

const std::unordered_map<std::string, flags::puppet_submode_type> SubmodeMap {
	{ "stand", flags::puppet_submode_type::Stand },
	{ "walk", flags::puppet_submode_type::Walk },
	{ "run", flags::puppet_submode_type::Run },
	{ "walkback", flags::puppet_submode_type::WalkBack },
	{ "strafeleft", flags::puppet_submode_type::StrafeLeft },
	{ "straferight", flags::puppet_submode_type::StrafeRight },
	{ "death", flags::puppet_submode_type::Death },
	{ "death2", flags::puppet_submode_type::Death2 },
	{ "fire", flags::puppet_submode_type::Fire },
	{ "hit", flags::puppet_submode_type::Hit },
	{ "rising", flags::puppet_submode_type::Rising },
	{ "drop", flags::puppet_submode_type::Drop },
	{ "fall", flags::puppet_submode_type::Fall },
	{ "activate", flags::puppet_submode_type::Activate },
	{ "crouchforward", flags::puppet_submode_type::CrouchForward },
	{ "toss", flags::puppet_submode_type::Toss },
	{ "place", flags::puppet_submode_type::Place },
	{ "magic", flags::puppet_submode_type::Magic },
	{ "fire2", flags::puppet_submode_type::Fire2 },
	{ "choke", flags::puppet_submode_type::Choke },
	{ "leap", flags::puppet_submode_type::Leap },
	{ "turnleft", flags::puppet_submode_type::TurnLeft },
	{ "turnright", flags::puppet_submode_type::TurnRight },
	{ "crouchback", flags::puppet_submode_type::CrouchBack },
	{ "fidget", flags::puppet_submode_type::Fidget },
	{ "fidget2", flags::puppet_submode_type::Fidget2 },
	{ "land", flags::puppet_submode_type::Land },
	{ "magic2", flags::puppet_submode_type::Magic2 },
	{ "magic3", flags::puppet_submode_type::Magic3 },
	{ "windup", flags::puppet_submode_type::Windup },
	{ "holster", flags::puppet_submode_type::Holster },
	{ "drawfists", flags::puppet_submode_type::DrawFists },
	{ "drawgun", flags::puppet_submode_type::DrawGun },
	{ "drawsaber", flags::puppet_submode_type::DrawSaber }
};

}
}
}

std::unique_ptr<gorc::content::asset> gorc::content::loaders::puppet_loader::parse(text::tokenizer& tok, manager& manager, diagnostics::report& report) {
	std::unique_ptr<assets::puppet> pup(new assets::puppet());

	text::token t;

	assets::puppet_mode* mode = nullptr;
	while(true) {
		tok.get_token(t);

		if(t.type == text::token_type::end_of_file) {
			break;
		}
		else if(boost::iequals(t.value, "mode")) {
			tok.set_report_eol(true);

			tok.assert_punctuator("=");
			mode = &pup->modes[tok.get_number<int>()];

			tok.get_token(t);
			if(t.type != text::token_type::end_of_line) {
				tok.assert_identifier("BasedOn");
				tok.assert_punctuator("=");
				*mode = pup->modes[tok.get_number<int>()];
			}

			tok.set_report_eol(false);
		}
		else if(boost::iequals(t.value, "joints")) {
			while(true) {
				tok.get_token(t);

				if(t.type == text::token_type::end_of_file || boost::iequals(t.value, "end")) {
					break;
				}
				else {
					tok.assert_punctuator("=");
					pup->joint_map.emplace(t.get_numeric_value<int>(), tok.get_number<int>());
				}
			}
		}
		else if(!mode) {
			report.add_warning("PuppetLoader::Parse", "expected \'mode\', \'joints\' or \'end\'", t.location);
			tok.skip_to_next_line();
		}
		else {
			std::transform(t.value.begin(), t.value.end(), t.value.begin(), tolower);
			auto it = SubmodeMap.find(t.value);
			if(it == SubmodeMap.end()) {
				report.add_warning("PuppetLoader::Parse",
						boost::str(boost::format("unknown submode %s") % t.value), t.location);
				tok.skip_to_next_line();
			}
			else {
				assets::puppet_submode& submode = mode->submodes[static_cast<unsigned int>(it->second)];
				submode.anim = &manager.load<assets::animation>(tok.get_space_delimited_string());
				submode.flags = flag_set<flags::key_flag>(tok.get_number<unsigned int>());
				submode.lo_priority = tok.get_number<unsigned int>();
				submode.hi_priority = tok.get_number<unsigned int>();
			}
		}
	}

	return std::unique_ptr<asset>(std::move(pup));
}
