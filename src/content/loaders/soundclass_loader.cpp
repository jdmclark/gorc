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

const std::unordered_map<std::string, Assets::SoundSubclassType> SubclassMap {
	{ "reserved", Assets::SoundSubclassType::Reserved },

	{ "create", Assets::SoundSubclassType::Create },
	{ "activate", Assets::SoundSubclassType::Activate },

	{ "startmove", Assets::SoundSubclassType::StartMove },
	{ "stopmove", Assets::SoundSubclassType::StopMove },
	{ "moving", Assets::SoundSubclassType::Moving },

	{ "lwalkhard", Assets::SoundSubclassType::LWalkHard },
	{ "rwalkhard", Assets::SoundSubclassType::RWalkHard },

	{ "lrunhard", Assets::SoundSubclassType::LRunHard },
	{ "rrunhard", Assets::SoundSubclassType::RRunHard },

	{ "lwalkmetal", Assets::SoundSubclassType::LWalkMetal },
	{ "rwalkmetal", Assets::SoundSubclassType::RWalkMetal },

	{ "lrunmetal", Assets::SoundSubclassType::LRunMetal },
	{ "rrunmetal", Assets::SoundSubclassType::RRunMetal },

	{ "lwalkwater", Assets::SoundSubclassType::LWalkWater },
	{ "rwalkwater", Assets::SoundSubclassType::RWalkWater },

	{ "lrunwater", Assets::SoundSubclassType::LRunWater },
	{ "rrunwater", Assets::SoundSubclassType::RRunWater },

	{ "lwalkpuddle", Assets::SoundSubclassType::LWalkPuddle },
	{ "rwalkpuddle", Assets::SoundSubclassType::RWalkPuddle },

	{ "lrunpuddle", Assets::SoundSubclassType::LRunPuddle },
	{ "rrunpuddle", Assets::SoundSubclassType::RRunPuddle },

	{ "lwalkearth", Assets::SoundSubclassType::LWalkEarth },
	{ "rwalkearth", Assets::SoundSubclassType::RWalkEarth },

	{ "lrunearth", Assets::SoundSubclassType::LRunEarth },
	{ "rrunearth", Assets::SoundSubclassType::RRunEarth },

	{ "enterwater", Assets::SoundSubclassType::EnterWater },
	{ "enterwaterslow", Assets::SoundSubclassType::EnterWaterSlow },
	{ "exitwater", Assets::SoundSubclassType::ExitWater },
	{ "exitwaterslow", Assets::SoundSubclassType::ExitWaterSlow },

	{ "lswimsurface", Assets::SoundSubclassType::LSwimSurface },
	{ "rswimsurface", Assets::SoundSubclassType::RSwimSurface },

	{ "treadsurface", Assets::SoundSubclassType::TreadSurface },

	{ "lswimunder", Assets::SoundSubclassType::LSwimUnder },
	{ "rswimunder", Assets::SoundSubclassType::RSwimUnder },

	{ "treadunder", Assets::SoundSubclassType::TreadUnder },

	{ "jump", Assets::SoundSubclassType::Jump },
	{ "jumpmetal", Assets::SoundSubclassType::JumpMetal },
	{ "jumpearth", Assets::SoundSubclassType::JumpEarth },
	{ "jumpwater", Assets::SoundSubclassType::JumpWater },

	{ "landhard", Assets::SoundSubclassType::LandHard },
	{ "landmetal", Assets::SoundSubclassType::LandMetal },
	{ "landwater", Assets::SoundSubclassType::LandWater },
	{ "landpuddle", Assets::SoundSubclassType::LandPuddle },
	{ "landearth", Assets::SoundSubclassType::LandEarth },
	{ "landhurt", Assets::SoundSubclassType::LandHurt },

	{ "hithard", Assets::SoundSubclassType::HitHard },
	{ "hitmetal", Assets::SoundSubclassType::HitMetal },
	{ "hitearth", Assets::SoundSubclassType::HitEarth },

	{ "deflected", Assets::SoundSubclassType::Deflected },

	{ "scrapehard", Assets::SoundSubclassType::ScrapeHard },
	{ "scrapemetal", Assets::SoundSubclassType::ScrapeMetal },
	{ "scrapeearth", Assets::SoundSubclassType::ScrapeEarth },

	{ "hitdamaged", Assets::SoundSubclassType::HitDamaged },

	{ "falling", Assets::SoundSubclassType::Falling },

	{ "corpsehit", Assets::SoundSubclassType::CorpseHit },

	{ "hurtimpact", Assets::SoundSubclassType::HurtImpact },
	{ "hurtenergy", Assets::SoundSubclassType::HurtEnergy },
	{ "hurtfire", Assets::SoundSubclassType::HurtFire },
	{ "hurtmagic", Assets::SoundSubclassType::HurtMagic },
	{ "hurtspecial", Assets::SoundSubclassType::HurtSpecial },
	{ "drowning", Assets::SoundSubclassType::Drowning },
	{ "choking", Assets::SoundSubclassType::Choking },
	{ "death1", Assets::SoundSubclassType::Death1 },
	{ "death2", Assets::SoundSubclassType::Death2 },
	{ "deathunder", Assets::SoundSubclassType::DeathUnder },
	{ "drowned", Assets::SoundSubclassType::Drowned },
	{ "splattered", Assets::SoundSubclassType::Splattered },

	{ "pant", Assets::SoundSubclassType::Pant },
	{ "breath", Assets::SoundSubclassType::Breath },
	{ "gasp", Assets::SoundSubclassType::Gasp },

	{ "fire1", Assets::SoundSubclassType::Fire1 },
	{ "fire2", Assets::SoundSubclassType::Fire2 },
	{ "fire3", Assets::SoundSubclassType::Fire3 },
	{ "fire4", Assets::SoundSubclassType::Fire4 },

	{ "curious", Assets::SoundSubclassType::Curious },
	{ "alert", Assets::SoundSubclassType::Alert },
	{ "idle", Assets::SoundSubclassType::Idle },
	{ "gloat", Assets::SoundSubclassType::Gloat },
	{ "fear", Assets::SoundSubclassType::Fear },
	{ "boast", Assets::SoundSubclassType::Boast },
	{ "happy", Assets::SoundSubclassType::Happy },
	{ "victory", Assets::SoundSubclassType::Victory },
	{ "help", Assets::SoundSubclassType::Help },
	{ "flee", Assets::SoundSubclassType::Flee },
	{ "search", Assets::SoundSubclassType::Search },
	{ "calm", Assets::SoundSubclassType::Calm },
	{ "surprise", Assets::SoundSubclassType::Surprise },

	{ "reserved1", Assets::SoundSubclassType::Reserved1 },
	{ "reserved2", Assets::SoundSubclassType::Reserved2 },
	{ "reserved3", Assets::SoundSubclassType::Reserved3 },
	{ "reserved4", Assets::SoundSubclassType::Reserved4 },
	{ "reserved5", Assets::SoundSubclassType::Reserved5 },
	{ "reserved6", Assets::SoundSubclassType::Reserved6 },
	{ "reserved7", Assets::SoundSubclassType::Reserved7 },
	{ "reserved8", Assets::SoundSubclassType::Reserved8 }
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

			Assets::SoundSubclassType st = it->second;
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
				subclass.flags = FlagSet<Assets::SoundFlag>(t.GetNumericValue<int>());
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
