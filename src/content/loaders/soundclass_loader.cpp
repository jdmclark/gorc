#include "soundclass_loader.h"
#include "content/assets/soundclass.h"
#include "content/manager.h"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <boost/format.hpp>

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::SoundClassLoader::AssetRootPath = { "misc/snd" };

namespace Gorc {
namespace Content {
namespace Loaders {

const std::unordered_map<std::string, Flags::SoundSubclassType> SubclassMap {
	{ "reserved", Flags::SoundSubclassType::Reserved },

	{ "create", Flags::SoundSubclassType::Create },
	{ "activate", Flags::SoundSubclassType::Activate },

	{ "startmove", Flags::SoundSubclassType::StartMove },
	{ "stopmove", Flags::SoundSubclassType::StopMove },
	{ "moving", Flags::SoundSubclassType::Moving },

	{ "lwalkhard", Flags::SoundSubclassType::LWalkHard },
	{ "rwalkhard", Flags::SoundSubclassType::RWalkHard },

	{ "lrunhard", Flags::SoundSubclassType::LRunHard },
	{ "rrunhard", Flags::SoundSubclassType::RRunHard },

	{ "lwalkmetal", Flags::SoundSubclassType::LWalkMetal },
	{ "rwalkmetal", Flags::SoundSubclassType::RWalkMetal },

	{ "lrunmetal", Flags::SoundSubclassType::LRunMetal },
	{ "rrunmetal", Flags::SoundSubclassType::RRunMetal },

	{ "lwalkwater", Flags::SoundSubclassType::LWalkWater },
	{ "rwalkwater", Flags::SoundSubclassType::RWalkWater },

	{ "lrunwater", Flags::SoundSubclassType::LRunWater },
	{ "rrunwater", Flags::SoundSubclassType::RRunWater },

	{ "lwalkpuddle", Flags::SoundSubclassType::LWalkPuddle },
	{ "rwalkpuddle", Flags::SoundSubclassType::RWalkPuddle },

	{ "lrunpuddle", Flags::SoundSubclassType::LRunPuddle },
	{ "rrunpuddle", Flags::SoundSubclassType::RRunPuddle },

	{ "lwalkearth", Flags::SoundSubclassType::LWalkEarth },
	{ "rwalkearth", Flags::SoundSubclassType::RWalkEarth },

	{ "lrunearth", Flags::SoundSubclassType::LRunEarth },
	{ "rrunearth", Flags::SoundSubclassType::RRunEarth },

	{ "enterwater", Flags::SoundSubclassType::EnterWater },
	{ "enterwaterslow", Flags::SoundSubclassType::EnterWaterSlow },
	{ "exitwater", Flags::SoundSubclassType::ExitWater },
	{ "exitwaterslow", Flags::SoundSubclassType::ExitWaterSlow },

	{ "lswimsurface", Flags::SoundSubclassType::LSwimSurface },
	{ "rswimsurface", Flags::SoundSubclassType::RSwimSurface },

	{ "treadsurface", Flags::SoundSubclassType::TreadSurface },

	{ "lswimunder", Flags::SoundSubclassType::LSwimUnder },
	{ "rswimunder", Flags::SoundSubclassType::RSwimUnder },

	{ "treadunder", Flags::SoundSubclassType::TreadUnder },

	{ "jump", Flags::SoundSubclassType::Jump },
	{ "jumpmetal", Flags::SoundSubclassType::JumpMetal },
	{ "jumpearth", Flags::SoundSubclassType::JumpEarth },
	{ "jumpwater", Flags::SoundSubclassType::JumpWater },

	{ "landhard", Flags::SoundSubclassType::LandHard },
	{ "landmetal", Flags::SoundSubclassType::LandMetal },
	{ "landwater", Flags::SoundSubclassType::LandWater },
	{ "landpuddle", Flags::SoundSubclassType::LandPuddle },
	{ "landearth", Flags::SoundSubclassType::LandEarth },
	{ "landhurt", Flags::SoundSubclassType::LandHurt },

	{ "hithard", Flags::SoundSubclassType::HitHard },
	{ "hitmetal", Flags::SoundSubclassType::HitMetal },
	{ "hitearth", Flags::SoundSubclassType::HitEarth },

	{ "deflected", Flags::SoundSubclassType::Deflected },

	{ "scrapehard", Flags::SoundSubclassType::ScrapeHard },
	{ "scrapemetal", Flags::SoundSubclassType::ScrapeMetal },
	{ "scrapeearth", Flags::SoundSubclassType::ScrapeEarth },

	{ "hitdamaged", Flags::SoundSubclassType::HitDamaged },

	{ "falling", Flags::SoundSubclassType::Falling },

	{ "corpsehit", Flags::SoundSubclassType::CorpseHit },

	{ "hurtimpact", Flags::SoundSubclassType::HurtImpact },
	{ "hurtenergy", Flags::SoundSubclassType::HurtEnergy },
	{ "hurtfire", Flags::SoundSubclassType::HurtFire },
	{ "hurtmagic", Flags::SoundSubclassType::HurtMagic },
	{ "hurtspecial", Flags::SoundSubclassType::HurtSpecial },
	{ "drowning", Flags::SoundSubclassType::Drowning },
	{ "choking", Flags::SoundSubclassType::Choking },
	{ "death1", Flags::SoundSubclassType::Death1 },
	{ "death2", Flags::SoundSubclassType::Death2 },
	{ "deathunder", Flags::SoundSubclassType::DeathUnder },
	{ "drowned", Flags::SoundSubclassType::Drowned },
	{ "splattered", Flags::SoundSubclassType::Splattered },

	{ "pant", Flags::SoundSubclassType::Pant },
	{ "breath", Flags::SoundSubclassType::Breath },
	{ "gasp", Flags::SoundSubclassType::Gasp },

	{ "fire1", Flags::SoundSubclassType::Fire1 },
	{ "fire2", Flags::SoundSubclassType::Fire2 },
	{ "fire3", Flags::SoundSubclassType::Fire3 },
	{ "fire4", Flags::SoundSubclassType::Fire4 },

	{ "curious", Flags::SoundSubclassType::Curious },
	{ "alert", Flags::SoundSubclassType::Alert },
	{ "idle", Flags::SoundSubclassType::Idle },
	{ "gloat", Flags::SoundSubclassType::Gloat },
	{ "fear", Flags::SoundSubclassType::Fear },
	{ "boast", Flags::SoundSubclassType::Boast },
	{ "happy", Flags::SoundSubclassType::Happy },
	{ "victory", Flags::SoundSubclassType::Victory },
	{ "help", Flags::SoundSubclassType::Help },
	{ "flee", Flags::SoundSubclassType::Flee },
	{ "search", Flags::SoundSubclassType::Search },
	{ "calm", Flags::SoundSubclassType::Calm },
	{ "surprise", Flags::SoundSubclassType::Surprise },

	{ "reserved1", Flags::SoundSubclassType::Reserved1 },
	{ "reserved2", Flags::SoundSubclassType::Reserved2 },
	{ "reserved3", Flags::SoundSubclassType::Reserved3 },
	{ "reserved4", Flags::SoundSubclassType::Reserved4 },
	{ "reserved5", Flags::SoundSubclassType::Reserved5 },
	{ "reserved6", Flags::SoundSubclassType::Reserved6 },
	{ "reserved7", Flags::SoundSubclassType::Reserved7 },
	{ "reserved8", Flags::SoundSubclassType::Reserved8 }
};

}
}
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::SoundClassLoader::Parse(Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Assets::SoundClass> snd(new Assets::SoundClass());

	Text::Token t;
	while(true) {
		tok.GetToken(t);

		if(t.Type == Text::TokenType::EndOfFile) {
			break;
		}
		else {
			std::transform(t.Value.begin(), t.Value.end(), t.Value.begin(), tolower);
			auto it = SubclassMap.find(t.Value);
			if(it == SubclassMap.end()) {
				report.AddWarning("SoundClassLoader::Parse",
						boost::str(boost::format("unknown subclass %s") % t.Value), t.Location);
				tok.SkipToNextLine();
				continue;
			}

			Flags::SoundSubclassType st = it->second;
			Assets::SoundSubclass& subclass = snd->Subclasses[static_cast<int>(st)];

			tok.SetReportEOL(true);

			try {
				subclass.sound = manager.LoadId<Assets::Sound>(tok.GetSpaceDelimitedString());
			}
			catch(...) {
				// Sound file not loaded. Set to invalid value.
				subclass.sound = -1;
			}

			tok.GetToken(t);
			if(t.Type != Text::TokenType::EndOfLine) {
				subclass.flags = FlagSet<Flags::SoundFlag>(t.GetNumericValue<int>());
			}
			else {
				tok.SetReportEOL(false);
				continue;
			}

			tok.GetToken(t);
			if(t.Type != Text::TokenType::EndOfLine) {
				subclass.min_radius = t.GetNumericValue<float>();
			}
			else {
				tok.SetReportEOL(false);
				continue;
			}

			tok.GetToken(t);
			if(t.Type != Text::TokenType::EndOfLine) {
				subclass.max_radius = t.GetNumericValue<float>();
			}
			else {
				tok.SetReportEOL(false);
				continue;
			}

			tok.GetToken(t);
			if(t.Type != Text::TokenType::EndOfLine) {
				subclass.max_volume = t.GetNumericValue<float>();
			}

			tok.SetReportEOL(false);
		}
	}

	return std::unique_ptr<Asset>(std::move(snd));
}
