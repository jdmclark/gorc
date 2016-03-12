#include "log/log.hpp"
#include "soundclass_loader.hpp"
#include "libold/content/assets/soundclass.hpp"
#include "libold/base/content/content_manager.hpp"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <boost/format.hpp>

gorc::fourcc const gorc::content::loaders::soundclass_loader::type = "SND"_4CC;

namespace {
    const std::vector<gorc::path> asset_root_path = { "misc/snd" };
}

std::vector<gorc::path> const& gorc::content::loaders::soundclass_loader::get_prefixes() const
{
    return asset_root_path;
}

namespace gorc {
namespace content {
namespace loaders {

const std::unordered_map<std::string, flags::sound_subclass_type> SubclassMap {
    { "reserved", flags::sound_subclass_type::Reserved },

    { "create", flags::sound_subclass_type::create },
    { "activate", flags::sound_subclass_type::Activate },

    { "startmove", flags::sound_subclass_type::StartMove },
    { "stopmove", flags::sound_subclass_type::StopMove },
    { "moving", flags::sound_subclass_type::Moving },

    { "lwalkhard", flags::sound_subclass_type::LWalkHard },
    { "rwalkhard", flags::sound_subclass_type::RWalkHard },

    { "lrunhard", flags::sound_subclass_type::LRunHard },
    { "rrunhard", flags::sound_subclass_type::RRunHard },

    { "lwalkmetal", flags::sound_subclass_type::LWalkMetal },
    { "rwalkmetal", flags::sound_subclass_type::RWalkMetal },

    { "lrunmetal", flags::sound_subclass_type::LRunMetal },
    { "rrunmetal", flags::sound_subclass_type::RRunMetal },

    { "lwalkwater", flags::sound_subclass_type::LWalkWater },
    { "rwalkwater", flags::sound_subclass_type::RWalkWater },

    { "lrunwater", flags::sound_subclass_type::LRunWater },
    { "rrunwater", flags::sound_subclass_type::RRunWater },

    { "lwalkpuddle", flags::sound_subclass_type::LWalkPuddle },
    { "rwalkpuddle", flags::sound_subclass_type::RWalkPuddle },

    { "lrunpuddle", flags::sound_subclass_type::LRunPuddle },
    { "rrunpuddle", flags::sound_subclass_type::RRunPuddle },

    { "lwalkearth", flags::sound_subclass_type::LWalkEarth },
    { "rwalkearth", flags::sound_subclass_type::RWalkEarth },

    { "lrunearth", flags::sound_subclass_type::LRunEarth },
    { "rrunearth", flags::sound_subclass_type::RRunEarth },

    { "enterwater", flags::sound_subclass_type::EnterWater },
    { "enterwaterslow", flags::sound_subclass_type::EnterWaterSlow },
    { "exitwater", flags::sound_subclass_type::ExitWater },
    { "exitwaterslow", flags::sound_subclass_type::ExitWaterSlow },

    { "lswimsurface", flags::sound_subclass_type::LSwimSurface },
    { "rswimsurface", flags::sound_subclass_type::RSwimSurface },

    { "treadsurface", flags::sound_subclass_type::TreadSurface },

    { "lswimunder", flags::sound_subclass_type::LSwimUnder },
    { "rswimunder", flags::sound_subclass_type::RSwimUnder },

    { "treadunder", flags::sound_subclass_type::TreadUnder },

    { "jump", flags::sound_subclass_type::Jump },
    { "jumpmetal", flags::sound_subclass_type::JumpMetal },
    { "jumpearth", flags::sound_subclass_type::JumpEarth },
    { "jumpwater", flags::sound_subclass_type::JumpWater },

    { "landhard", flags::sound_subclass_type::LandHard },
    { "landmetal", flags::sound_subclass_type::LandMetal },
    { "landwater", flags::sound_subclass_type::LandWater },
    { "landpuddle", flags::sound_subclass_type::LandPuddle },
    { "landearth", flags::sound_subclass_type::LandEarth },
    { "landhurt", flags::sound_subclass_type::LandHurt },

    { "hithard", flags::sound_subclass_type::HitHard },
    { "hitmetal", flags::sound_subclass_type::HitMetal },
    { "hitearth", flags::sound_subclass_type::HitEarth },

    { "deflected", flags::sound_subclass_type::deflected },

    { "scrapehard", flags::sound_subclass_type::ScrapeHard },
    { "scrapemetal", flags::sound_subclass_type::ScrapeMetal },
    { "scrapeearth", flags::sound_subclass_type::ScrapeEarth },

    { "hitdamaged", flags::sound_subclass_type::HitDamaged },

    { "falling", flags::sound_subclass_type::Falling },

    { "corpsehit", flags::sound_subclass_type::CorpseHit },

    { "hurtimpact", flags::sound_subclass_type::HurtImpact },
    { "hurtenergy", flags::sound_subclass_type::HurtEnergy },
    { "hurtfire", flags::sound_subclass_type::HurtFire },
    { "hurtmagic", flags::sound_subclass_type::HurtMagic },
    { "hurtspecial", flags::sound_subclass_type::HurtSpecial },
    { "drowning", flags::sound_subclass_type::Drowning },
    { "choking", flags::sound_subclass_type::Choking },
    { "death1", flags::sound_subclass_type::Death1 },
    { "death2", flags::sound_subclass_type::Death2 },
    { "deathunder", flags::sound_subclass_type::DeathUnder },
    { "drowned", flags::sound_subclass_type::Drowned },
    { "splattered", flags::sound_subclass_type::Splattered },

    { "pant", flags::sound_subclass_type::Pant },
    { "breath", flags::sound_subclass_type::Breath },
    { "gasp", flags::sound_subclass_type::Gasp },

    { "fire1", flags::sound_subclass_type::Fire1 },
    { "fire2", flags::sound_subclass_type::Fire2 },
    { "fire3", flags::sound_subclass_type::Fire3 },
    { "fire4", flags::sound_subclass_type::Fire4 },

    { "curious", flags::sound_subclass_type::Curious },
    { "alert", flags::sound_subclass_type::Alert },
    { "idle", flags::sound_subclass_type::Idle },
    { "gloat", flags::sound_subclass_type::Gloat },
    { "fear", flags::sound_subclass_type::Fear },
    { "boast", flags::sound_subclass_type::Boast },
    { "happy", flags::sound_subclass_type::Happy },
    { "victory", flags::sound_subclass_type::Victory },
    { "help", flags::sound_subclass_type::Help },
    { "flee", flags::sound_subclass_type::Flee },
    { "search", flags::sound_subclass_type::Search },
    { "calm", flags::sound_subclass_type::Calm },
    { "surprise", flags::sound_subclass_type::Surprise },

    { "reserved1", flags::sound_subclass_type::Reserved1 },
    { "reserved2", flags::sound_subclass_type::Reserved2 },
    { "reserved3", flags::sound_subclass_type::Reserved3 },
    { "reserved4", flags::sound_subclass_type::Reserved4 },
    { "reserved5", flags::sound_subclass_type::Reserved5 },
    { "reserved6", flags::sound_subclass_type::Reserved6 },
    { "reserved7", flags::sound_subclass_type::Reserved7 },
    { "reserved8", flags::sound_subclass_type::Reserved8 }
};

}
}
}

std::unique_ptr<gorc::asset> gorc::content::loaders::soundclass_loader::parse(text::tokenizer& tok, content_manager& manager, service_registry const &) const {
    std::unique_ptr<assets::soundclass> snd(new assets::soundclass());

    text::token t;
    while(true) {
        tok.get_token(t);

        if(t.type == text::token_type::end_of_file) {
            break;
        }
        else {
            std::transform(t.value.begin(), t.value.end(), t.value.begin(), tolower);
            auto it = SubclassMap.find(t.value);
            if(it == SubclassMap.end()) {
                diagnostic_context dc(nullptr, t.location.first_line, t.location.last_line);
                LOG_WARNING(format("unknown subclass %s") % t.value);
                tok.skip_to_next_line();
                continue;
            }

            flags::sound_subclass_type st = it->second;
            assets::sound_subclass& subclass = snd->subclasses[static_cast<int>(st)];

            tok.set_report_eol(true);

            try {
                subclass.sound = manager.load_id<assets::sound>(tok.get_space_delimited_string());
            }
            catch(...) {
                // sound file not loaded. Set to invalid value.
                subclass.sound = -1;
            }

            tok.get_token(t);
            if(t.type != text::token_type::end_of_line) {
                subclass.flags = flag_set<flags::sound_flag>(t.get_numeric_value<int>());
            }
            else {
                tok.set_report_eol(false);
                continue;
            }

            tok.get_token(t);
            if(t.type != text::token_type::end_of_line) {
                subclass.min_radius = t.get_numeric_value<float>();
            }
            else {
                tok.set_report_eol(false);
                continue;
            }

            tok.get_token(t);
            if(t.type != text::token_type::end_of_line) {
                subclass.max_radius = t.get_numeric_value<float>();
            }
            else {
                tok.set_report_eol(false);
                continue;
            }

            tok.get_token(t);
            if(t.type != text::token_type::end_of_line) {
                subclass.max_volume = t.get_numeric_value<float>();
            }

            tok.set_report_eol(false);
        }
    }

    return std::unique_ptr<asset>(std::move(snd));
}
