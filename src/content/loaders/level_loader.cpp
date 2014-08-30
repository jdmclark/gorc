#include "level_loader.h"
#include "content/assets/level.h"
#include "base/content/content_manager.h"
#include "content/constants.h"
#include "base/math/vector.h"
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>

const std::vector<boost::filesystem::path> gorc::content::loaders::level_loader::asset_root_path = { "jkl", "mission" };

namespace gorc {
namespace content {
namespace loaders {

void SkipToNextSection(text::tokenizer& tok) {
    text::token t;
    do {
        tok.get_token(t);
    }
    while(t.type != text::token_type::end_of_file && (t.type != text::token_type::identifier || !boost::iequals(t.value, "SECTION")));

    if(t.type == text::token_type::end_of_file) {
        return;
    }

    tok.assert_punctuator(":");
}

void ParseJKSection(assets::level&, text::tokenizer&, content_manager&, cog::compiler&, diagnostics::report&) {
    // JK section is empty.
    return;
}

void ParseHeaderSection(assets::level& lev, text::tokenizer& tok, content_manager&, cog::compiler&, diagnostics::report&) {
    tok.assert_identifier("Version");
    lev.header.version = tok.get_number<int>();

    tok.assert_identifier("world");
    tok.assert_identifier("Gravity");
    lev.header.world_gravity = tok.get_number<float>();

    tok.assert_identifier("Ceiling");
    tok.assert_identifier("Sky");
    tok.assert_identifier("Z");
    lev.header.ceiling_sky_z = tok.get_number<float>();

    tok.assert_identifier("Horizon");
    tok.assert_identifier("Distance");
    lev.header.horizon_distance = tok.get_number<float>();

    tok.assert_identifier("Horizon");
    tok.assert_identifier("Pixels");
    tok.assert_identifier("Per");
    tok.assert_identifier("Rev");
    lev.header.horizon_pixels_per_rev = tok.get_number<float>();

    tok.assert_identifier("Horizon");
    tok.assert_identifier("Sky");
    tok.assert_identifier("Offset");
    get<0>(lev.header.horizon_sky_offset) = tok.get_number<float>();
    get<1>(lev.header.horizon_sky_offset) = tok.get_number<float>();

    tok.assert_identifier("Ceiling");
    tok.assert_identifier("Sky");
    tok.assert_identifier("Offset");
    get<0>(lev.header.ceiling_sky_offset) = tok.get_number<float>();
    get<1>(lev.header.ceiling_sky_offset) = tok.get_number<float>();

    tok.assert_identifier("MipMap");
    tok.assert_identifier("Distances");
    std::get<0>(lev.header.mipmap_distances) = tok.get_number<float>();
    std::get<1>(lev.header.mipmap_distances) = tok.get_number<float>();
    std::get<2>(lev.header.mipmap_distances) = tok.get_number<float>();
    std::get<3>(lev.header.mipmap_distances) = tok.get_number<float>();

    tok.assert_identifier("LOD");
    tok.assert_identifier("Distances");
    std::get<0>(lev.header.lod_distances) = tok.get_number<float>();
    std::get<1>(lev.header.lod_distances) = tok.get_number<float>();
    std::get<2>(lev.header.lod_distances) = tok.get_number<float>();
    std::get<3>(lev.header.lod_distances) = tok.get_number<float>();

    tok.assert_identifier("Perspective");
    tok.assert_identifier("distance");
    lev.header.perspective_distance = tok.get_number<float>();

    tok.assert_identifier("Gouraud");
    tok.assert_identifier("distance");
    lev.header.gouraud_distance = tok.get_number<float>();
}

void ParseMaterialsSection(assets::level& lev, text::tokenizer& tok, content_manager&, cog::compiler&, diagnostics::report& report) {
    tok.assert_identifier("world");
    tok.assert_identifier("materials");

    size_t num = tok.get_number<size_t>();

    text::token t;
    while(true) {
        tok.get_token(t);

        if(t.type == text::token_type::identifier && boost::iequals(t.value, "end")) {
            break;
        }
        else {
            tok.assert_punctuator(":");

            t.value = tok.get_space_delimited_string();
            float x_scale = tok.get_number<float>();
            float y_scale = tok.get_number<float>();

            lev.materials.emplace_back(nullptr, x_scale, y_scale, t.value);
        }
    }

    if(num != lev.materials.size()) {
        report.add_warning("LevelLoader::ParseMaterialsSection",
                boost::str(boost::format("expected %d materials, found %d entries") % num % lev.materials.size()),
                t.location);
    }
}

void ParseGeoresourceSection(assets::level& lev, text::tokenizer& tok, content_manager& manager, cog::compiler&, diagnostics::report&) {
    text::token t;

    tok.assert_identifier("World");
    tok.assert_identifier("Colormaps");

    size_t num_colormaps = tok.get_number<size_t>();

    for(size_t i = 0; i < num_colormaps; ++i) {
        tok.get_number<int>();
        tok.assert_punctuator(":");

        lev.colormaps.push_back(&manager.load<assets::colormap>(tok.get_space_delimited_string()));
        if(!lev.master_colormap) {
            lev.master_colormap = lev.colormaps.back();
        }
    }

    tok.assert_identifier("World");
    tok.assert_identifier("Vertices");

    size_t num_vertices = tok.get_number<size_t>();

    for(size_t i = 0; i < num_vertices; ++i) {
        tok.get_number<int>();
        tok.assert_punctuator(":");

        float x = tok.get_number<float>();
        float y = tok.get_number<float>();
        float z = tok.get_number<float>();

        lev.vertices.push_back(make_vector(x, y, z));
    }

    tok.assert_identifier("World");
    tok.assert_identifier("Texture");
    tok.assert_identifier("Vertices");

    size_t num_tex_vert = tok.get_number<size_t>();

    for(size_t i = 0; i < num_tex_vert; ++i) {
        tok.get_number<int>();
        tok.assert_punctuator(":");

        float u = tok.get_number<float>();
        float v = tok.get_number<float>();

        lev.texture_vertices.push_back(make_vector(u, v));
    }

    tok.assert_identifier("World");
    tok.assert_identifier("Adjoins");

    size_t num_adjoins = tok.get_number<size_t>();

    for(size_t i = 0; i < num_adjoins; ++i) {
        tok.get_number<int>();
        tok.assert_punctuator(":");

        assets::level_adjoin adj;

        adj.flags = flag_set<flags::adjoin_flag>(tok.get_number<uint32_t>());
        adj.mirror = tok.get_number<int>();
        adj.distance = tok.get_number<float>();

        lev.adjoins.push_back(adj);
    }

    tok.assert_identifier("World");
    tok.assert_identifier("Surfaces");

    size_t num_surfaces = tok.get_number<size_t>();

    for(size_t i = 0; i < num_surfaces; ++i) {
        lev.surfaces.emplace_back();
        assets::level_surface& surf = lev.surfaces.back();

        tok.get_number<int>();
        tok.assert_punctuator(":");

        surf.material = tok.get_number<int>();
        surf.flags = flag_set<flags::surface_flag>(tok.get_number<uint32_t>());
        surf.face_type_flags = flag_set<flags::face_flag>(tok.get_number<uint32_t>());
        surf.geometry_mode = static_cast<flags::geometry_mode>(tok.get_number<uint32_t>());
        surf.light_mode = static_cast<flags::light_mode>(tok.get_number<uint32_t>());
        surf.texture_mode = static_cast<flags::texture_mode>(tok.get_number<uint32_t>());
        surf.adjoin = tok.get_number<int>();
        surf.extra_light = tok.get_number<float>();

        surf.vertices.clear();

        size_t nverts = tok.get_number<size_t>();
        for(size_t i = 0; i < nverts; ++i) {
            int vx = tok.get_number<int>();
            tok.assert_punctuator(",");
            int tx = tok.get_number<int>();

            surf.vertices.emplace_back(vx, tx, 0.0f);
        }

        for(auto& vx : surf.vertices) {
            std::get<2>(vx) = tok.get_number<float>();
        }
    }

    for(auto& surf : lev.surfaces) {
        tok.get_number<int>();
        tok.assert_punctuator(":");

        float x = tok.get_number<float>();
        float y = tok.get_number<float>();
        float z = tok.get_number<float>();

        surf.normal = make_vector(x, y, z);
    }
}

void ParseSectorsSection(assets::level& lev, text::tokenizer& tok, content_manager& manager, cog::compiler&, diagnostics::report& report) {
    text::token t;

    tok.assert_identifier("world");
    tok.assert_identifier("sectors");

    size_t num_sectors = tok.get_number<size_t>();

    for(size_t i = 0; i < num_sectors; ++i) {
        assets::level_sector sec;

        while(true) {
            tok.get_token(t);

            if(t.type == text::token_type::end_of_file) {
                report.add_error("LevelLoader::ParseSectorsSection", "unexpected end of file in sector", t.location);
                return;
            }
            if(boost::iequals(t.value, "sector")) {
                sec.number = tok.get_number<int>();
            }
            else if(boost::iequals(t.value, "flags")) {
                sec.flags = flag_set<flags::sector_flag>(tok.get_number<uint32_t>());
            }
            else if(boost::iequals(t.value, "ambient")) {
                tok.assert_identifier("light");
                sec.ambient_light = tok.get_number<float>();
            }
            else if(boost::iequals(t.value, "extra")) {
                tok.assert_identifier("light");
                sec.extra_light = tok.get_number<float>();
            }
            else if(boost::iequals(t.value, "colormap")) {
                sec.colormap_id = tok.get_number<int>();
            }
            else if(boost::iequals(t.value, "tint")) {
                float r = tok.get_number<float>();
                float g = tok.get_number<float>();
                float b = tok.get_number<float>();
                sec.tint = make_vector(r, g, b);
            }
            else if(boost::iequals(t.value, "boundbox")) {
                float x0 = tok.get_number<float>();
                float y0 = tok.get_number<float>();
                float z0 = tok.get_number<float>();
                float x1 = tok.get_number<float>();
                float y1 = tok.get_number<float>();
                float z1 = tok.get_number<float>();

                sec.bounding_box = box<3>(make_vector(x0, y0, z0), make_vector(x1, y1, z1));
            }
            else if(boost::iequals(t.value, "collidebox")) {
                float x0 = tok.get_number<float>();
                float y0 = tok.get_number<float>();
                float z0 = tok.get_number<float>();
                float x1 = tok.get_number<float>();
                float y1 = tok.get_number<float>();
                float z1 = tok.get_number<float>();

                sec.collide_box = box<3>(make_vector(x0, y0, z0), make_vector(x1, y1, z1));
            }
            else if(boost::iequals(t.value, "sound")) {
                sec.ambient_sound = &manager.load<assets::sound>(tok.get_space_delimited_string());
                sec.ambient_sound_volume = tok.get_number<float>();
            }
            else if(boost::iequals(t.value, "center")) {
                float x = tok.get_number<float>();
                float y = tok.get_number<float>();
                float z = tok.get_number<float>();
                sec.center = make_vector(x, y, z);
            }
            else if(boost::iequals(t.value, "radius")) {
                sec.radius = tok.get_number<float>();
            }
            else if(boost::iequals(t.value, "vertices")) {
                size_t num_vertices = tok.get_number<size_t>();
                for(size_t i = 0; i < num_vertices; ++i) {
                    tok.get_number<int>();
                    tok.assert_punctuator(":");
                    sec.vertices.push_back(tok.get_number<int>());
                }
            }
            else if(boost::iequals(t.value, "surfaces")) {
                sec.first_surface = tok.get_number<int>();
                sec.surface_count = tok.get_number<int>();
                break;
            }
            else {
                tok.skip_to_next_line();
                report.add_warning("LevelLoader::ParseSectorsSection",
                        boost::str(boost::format("unhandled sector field \'%s\'") % t.value),
                        t.location);
            }
        }

        lev.sectors.push_back(sec);
    }
}

void ParseCogsSection(assets::level& lev, text::tokenizer& tok, content_manager& manager, cog::compiler& compiler, diagnostics::report& report) {
    tok.assert_identifier("world");
    tok.assert_identifier("Cogs");

    size_t num = tok.get_number<size_t>();

    text::token t;
    while(true) {
        tok.get_token(t);

        if(t.type == text::token_type::identifier && boost::iequals(t.value, "end")) {
            break;
        }
        else {
            tok.assert_punctuator(":");

            assets::script const* script = nullptr;
            try {
                script = &manager.load<assets::script>(tok.get_space_delimited_string(), compiler);
            }
            catch(...) {
                // Failed to load script. Add empty entry and advance to next entry.
                lev.cogs.emplace_back(nullptr, std::vector<cog::vm::value>());
                tok.skip_to_next_line();
                continue;
            }

            std::vector<cog::vm::value> values;

            tok.set_report_eol(true);

            for(const cog::symbols::symbol& symbol : script->cogscript.symbol_table) {
                if(!symbol.local) {
                    switch(symbol.type) {
                    case cog::symbols::symbol_type::ai:
                    case cog::symbols::symbol_type::keyframe:
                    case cog::symbols::symbol_type::material:
                    case cog::symbols::symbol_type::model:
                    case cog::symbols::symbol_type::sound:
                    case cog::symbols::symbol_type::thing_template:
                    case cog::symbols::symbol_type::string:
                        lev.cog_strings.emplace_back(new std::string(tok.get_space_delimited_string()));
                        values.push_back(lev.cog_strings.back()->data());
                        break;

                    case cog::symbols::symbol_type::cog:
                    case cog::symbols::symbol_type::sector:
                    case cog::symbols::symbol_type::surface:
                    case cog::symbols::symbol_type::thing:
                    case cog::symbols::symbol_type::integer:
                        values.push_back(tok.get_number<int>());
                        break;

                    case cog::symbols::symbol_type::flex:
                    case cog::symbols::symbol_type::floating:
                        values.push_back(tok.get_number<float>());
                        break;

                    case cog::symbols::symbol_type::vector: {
                        tok.assert_punctuator("(");
                        float x = tok.get_number<float>();
                        tok.assert_punctuator("/");
                        float y = tok.get_number<float>();
                        tok.assert_punctuator("/");
                        float z = tok.get_number<float>();
                        tok.assert_punctuator(")");
                        values.push_back(make_vector(x, y, z));
                    }
                    break;

                    case cog::symbols::symbol_type::message:
                        // Ignore
                        break;

                    default:
                        report.add_warning("LevelLoader::ParseCogsSection", "unhandled symbol type", t.location);
                        continue;
                    }
                }
            }

            tok.set_report_eol(false);

            lev.cogs.emplace_back(script, values);
        }
    }

    if(num != lev.cogs.size()) {
        report.add_warning("LevelLoader::ParseCogsSection",
                boost::str(boost::format("expected %d cog instances, found %d entries") % num % lev.cogs.size()),
                t.location);
    }
}

void ParseTemplatesSection(assets::level& lev, text::tokenizer& tok, content_manager& manager, cog::compiler& compiler, diagnostics::report& report) {
    tok.assert_identifier("world");
    tok.assert_identifier("templates");

    // Add 'none' template to list with default parameters.
    lev.template_map.insert(std::make_pair("none", lev.templates.size()));
    lev.templates.push_back(assets::thing_template());

    size_t num = tok.get_number<size_t>();

    std::string tpl_name;
    std::string base_name;
    while(true) {
        tpl_name = tok.get_space_delimited_string();
        std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), tolower);

        if(tpl_name == "end") {
            break;
        }
        else if(!tpl_name.empty()) {
            base_name = tok.get_space_delimited_string();
            std::transform(base_name.begin(), base_name.end(), base_name.begin(), tolower);

            auto base_it = lev.template_map.find(base_name);
            if(base_it == lev.template_map.end()) {
                report.add_error("LevelLoader::ParseTemplatesSection",
                        boost::str(boost::format("template %s parent %s not defined") % tpl_name % base_name), tok.get_internal_token_location());
                tok.skip_to_next_line();
                continue;
            }

            auto succ_pair = lev.template_map.emplace(tpl_name, lev.templates.size());
            if(!succ_pair.second) {
                report.add_warning("LevelLoader::ParseTemplatesSection",
                        boost::str(boost::format("template %s redefinition") % tpl_name), tok.get_internal_token_location());
            }

            lev.templates.push_back(lev.templates[base_it->second]);
            lev.templates.back().parse_args(tok, manager, *lev.master_colormap, compiler, lev.template_map, report);
        }
        else {
            report.add_error("LevelLoader::ParseTemplatesSection", "expected template name", tok.get_internal_token_location());
            break;
        }
    }

    if(num != lev.templates.size()) {
        report.add_warning("LevelLoader::ParseTemplatesSection",
                boost::str(boost::format("expected %d templates, found %d entries") % num % lev.templates.size()),
                tok.get_internal_token_location());
    }
}

void ParseThingsSection(assets::level& lev, text::tokenizer& tok, content_manager& manager, cog::compiler& compiler, diagnostics::report& report) {
    tok.assert_identifier("world");
    tok.assert_identifier("things");

    size_t num = tok.get_number<size_t>();

    std::string tpl_name, thing_name;
    text::token t;
    while(true) {
        tok.get_token(t);

        if(t.type == text::token_type::identifier && boost::iequals(t.value, "end")) {
            break;
        }
        else {
            tok.assert_punctuator(":");

            tpl_name = tok.get_space_delimited_string();
            thing_name = tok.get_space_delimited_string();

            float x = tok.get_number<float>();
            float y = tok.get_number<float>();
            float z = tok.get_number<float>();
            float pitch = tok.get_number<float>();
            float yaw = tok.get_number<float>();
            float roll = tok.get_number<float>();
            int sector = tok.get_number<int>();

            auto base_it = lev.template_map.find(tpl_name);
            if(base_it == lev.template_map.end()) {
                report.add_error("LevelLoader::ParseThingsSection",
                        boost::str(boost::format("thing uses undefined template \'%s\'") % tpl_name),
                        tok.get_internal_token_location());
                tok.skip_to_next_line();
                continue;
            }

            lev.things.emplace_back(lev.templates[base_it->second]);
            lev.things.back().sector = sector;
            lev.things.back().position = make_vector(x, y, z);
            lev.things.back().orient = make_euler(make_vector(pitch, yaw, roll));
            lev.things.back().parse_args(tok, manager, *lev.master_colormap, compiler, lev.template_map, report);
        }
    }

    if(num != lev.things.size()) {
        report.add_warning("LevelLoader::ParseThingsSection",
                boost::str(boost::format("expected %d things, found %d entries") % num % lev.things.size()),
                t.location);
    }
}

using LevelSectionParser = std::function<void(assets::level&, text::tokenizer&, content_manager&, cog::compiler&, diagnostics::report&)>;
const std::unordered_map<std::string, LevelSectionParser> LevelSectionParserMap {
    {"jk", ParseJKSection},
    {"header", ParseHeaderSection},
    {"materials", ParseMaterialsSection},
    {"georesource", ParseGeoresourceSection},
    {"sectors", ParseSectorsSection},
    {"cogs", ParseCogsSection},
    {"templates", ParseTemplatesSection},
    {"things", ParseThingsSection}
};

void PostprocessLevel(assets::level& lev, content_manager& manager, cog::compiler&, diagnostics::report&) {
    // Post-process; load materials and scripts.
    for(auto& mat_entry : lev.materials) {
        std::get<0>(mat_entry) = &manager.load<assets::material>(std::get<3>(mat_entry), *lev.master_colormap);
    }

    // Add adjoined sector reference to adjoins.
    for(auto& surf : lev.surfaces) {
        if(surf.adjoin >= 0) {
            int mirror_adj = lev.adjoins[surf.adjoin].mirror;
            for(const auto& sec : lev.sectors) {
                bool found = false;
                for(int i = sec.first_surface; i < sec.first_surface + sec.surface_count; ++i) {
                    if(lev.surfaces[i].adjoin == mirror_adj) {
                        found = true;
                        surf.adjoined_sector = sec.number;
                        break;
                    }
                }

                if(found) {
                    break;
                }
            }
        }
    }

    // Calculate axis-aligned bounding box for each sector and assign colormap pointer.
    for(auto& sec : lev.sectors) {
        if(sec.colormap_id >= 0) {
            sec.cmp = lev.colormaps[sec.colormap_id];
        }

        vector<3> min_aabb = make_fill_vector<3>(std::numeric_limits<float>::max());
        vector<3> max_aabb = make_fill_vector<3>(std::numeric_limits<float>::lowest());

        for(auto& vx_id : sec.vertices) {
            auto& vx = lev.vertices[vx_id];

            for(auto vx_it = vx.begin(), min_it = min_aabb.begin(), max_it = max_aabb.begin();
                    vx_it != vx.end() && min_it != min_aabb.end() && max_it != max_aabb.end();
                    ++vx_it, ++min_it, ++max_it) {
                *min_it = std::min(*min_it, *vx_it);
                *max_it = std::max(*max_it, *vx_it);
            }
        }

        sec.bounding_box = box<3>(min_aabb, max_aabb);
        sec.collide_box = box<3>(min_aabb, max_aabb);
    }
}

}
}
}

std::unique_ptr<gorc::content::asset> gorc::content::loaders::level_loader::parse(text::tokenizer& tok, content_manager& manager, diagnostics::report& report) {
    std::unique_ptr<assets::level> lev(new assets::level());

    text::token t;
    while(true) {
        SkipToNextSection(tok);
        tok.get_token(t);

        if(t.type == text::token_type::end_of_file) {
            break;
        }
        else {
            std::transform(t.value.begin(), t.value.end(), t.value.begin(), tolower);
            auto it = LevelSectionParserMap.find(t.value);
            if(it == LevelSectionParserMap.end()) {
                report.add_warning("LevelLoader", boost::str(boost::format("skipping unknown section %s") % t.value), t.location);
            }
            else {
                it->second(*lev, tok, manager, compiler, report);
            }
        }
    }

    PostprocessLevel(*lev, manager, compiler, report);

    return std::unique_ptr<asset>(std::move(lev));
}

gorc::content::loaders::level_loader::level_loader(cog::compiler& compiler)
    : compiler(compiler) {
    return;
}
