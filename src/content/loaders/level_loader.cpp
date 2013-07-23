#include "level_loader.h"
#include "content/assets/level.h"
#include "content/manager.h"
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::LevelLoader::AssetRootPath = { "jkl" };

namespace Gorc {
namespace Content {
namespace Loaders {

void SkipToNextSection(Text::Tokenizer& tok) {
	Text::Token t;
	do {
		tok.GetToken(t);
	}
	while(t.Type != Text::TokenType::EndOfFile && (t.Type != Text::TokenType::Identifier || !boost::iequals(t.Value, "SECTION")));

	if(t.Type == Text::TokenType::EndOfFile) {
		return;
	}

	tok.AssertPunctuator(":");
}

void ParseJKSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	// JK section is empty.
	return;
}

void ParseHeaderSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	tok.AssertIdentifier("Version");
	lev.Header.Version = tok.GetNumber<int>();

	tok.AssertIdentifier("World");
	tok.AssertIdentifier("Gravity");
	lev.Header.WorldGravity = tok.GetNumber<float>();

	tok.AssertIdentifier("Ceiling");
	tok.AssertIdentifier("Sky");
	tok.AssertIdentifier("Z");
	lev.Header.CeilingSkyZ = tok.GetNumber<float>();

	tok.AssertIdentifier("Horizon");
	tok.AssertIdentifier("Distance");
	lev.Header.HorizonDistance = tok.GetNumber<float>();

	tok.AssertIdentifier("Horizon");
	tok.AssertIdentifier("Pixels");
	tok.AssertIdentifier("Per");
	tok.AssertIdentifier("Rev");
	lev.Header.HorizonPixelsPerRev = tok.GetNumber<float>();

	tok.AssertIdentifier("Horizon");
	tok.AssertIdentifier("Sky");
	tok.AssertIdentifier("Offset");
	Math::Get<Math::X>(lev.Header.HorizonSkyOffset) = tok.GetNumber<float>();
	Math::Get<Math::Y>(lev.Header.HorizonSkyOffset) = tok.GetNumber<float>();

	tok.AssertIdentifier("Ceiling");
	tok.AssertIdentifier("Sky");
	tok.AssertIdentifier("Offset");
	Math::Get<Math::X>(lev.Header.CeilingSkyOffset) = tok.GetNumber<float>();
	Math::Get<Math::Y>(lev.Header.CeilingSkyOffset) = tok.GetNumber<float>();

	tok.AssertIdentifier("MipMap");
	tok.AssertIdentifier("Distances");
	std::get<0>(lev.Header.MipMapDistances) = tok.GetNumber<float>();
	std::get<1>(lev.Header.MipMapDistances) = tok.GetNumber<float>();
	std::get<2>(lev.Header.MipMapDistances) = tok.GetNumber<float>();
	std::get<3>(lev.Header.MipMapDistances) = tok.GetNumber<float>();

	tok.AssertIdentifier("LOD");
	tok.AssertIdentifier("Distances");
	std::get<0>(lev.Header.LODDistances) = tok.GetNumber<float>();
	std::get<1>(lev.Header.LODDistances) = tok.GetNumber<float>();
	std::get<2>(lev.Header.LODDistances) = tok.GetNumber<float>();
	std::get<3>(lev.Header.LODDistances) = tok.GetNumber<float>();

	tok.AssertIdentifier("Perspective");
	tok.AssertIdentifier("distance");
	lev.Header.PerspectiveDistance = tok.GetNumber<float>();

	tok.AssertIdentifier("Gouraud");
	tok.AssertIdentifier("distance");
	lev.Header.GouraudDistance = tok.GetNumber<float>();
}

void ParseMaterialsSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	tok.AssertIdentifier("World");
	tok.AssertIdentifier("materials");

	size_t num = tok.GetNumber<size_t>();

	Text::Token t;
	while(true) {
		tok.GetToken(t);

		if(t.Type == Text::TokenType::Identifier && boost::iequals(t.Value, "end")) {
			break;
		}
		else {
			tok.AssertPunctuator(":");

			t.Value = tok.GetSpaceDelimitedString();
			float x_scale = tok.GetNumber<float>();
			float y_scale = tok.GetNumber<float>();

			lev.Materials.emplace_back(nullptr, x_scale, y_scale, t.Value);
		}
	}

	if(num != lev.Materials.size()) {
		report.AddWarning("LevelLoader::ParseMaterialsSection",
				boost::str(boost::format("expected %d materials, found %d entries") % num % lev.Materials.size()),
				t.Location);
	}
}

void ParseGeoresourceSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	Text::Token t;

	tok.AssertIdentifier("World");
	tok.AssertIdentifier("Colormaps");

	size_t num_colormaps = tok.GetNumber<size_t>();

	for(size_t i = 0; i < num_colormaps; ++i) {
		tok.GetNumber<int>();
		tok.AssertPunctuator(":");

		lev.Colormaps.push_back(&manager.Load<Assets::Colormap>(tok.GetSpaceDelimitedString()));
		if(!lev.MasterColormap) {
			lev.MasterColormap = lev.Colormaps.back();
		}
	}

	tok.AssertIdentifier("World");
	tok.AssertIdentifier("Vertices");

	size_t num_vertices = tok.GetNumber<size_t>();

	for(size_t i = 0; i < num_vertices; ++i) {
		tok.GetNumber<int>();
		tok.AssertPunctuator(":");

		float x = tok.GetNumber<float>();
		float y = tok.GetNumber<float>();
		float z = tok.GetNumber<float>();

		lev.Vertices.push_back(Math::Vec(x, y, z));
	}

	tok.AssertIdentifier("World");
	tok.AssertIdentifier("texture");
	tok.AssertIdentifier("vertices");

	size_t num_tex_vert = tok.GetNumber<size_t>();

	for(size_t i = 0; i < num_tex_vert; ++i) {
		tok.GetNumber<int>();
		tok.AssertPunctuator(":");

		float u = tok.GetNumber<float>();
		float v = tok.GetNumber<float>();

		lev.TextureVertices.push_back(Math::Vec(u, v));
	}

	tok.AssertIdentifier("World");
	tok.AssertIdentifier("adjoins");

	size_t num_adjoins = tok.GetNumber<size_t>();

	for(size_t i = 0; i < num_adjoins; ++i) {
		tok.GetNumber<int>();
		tok.AssertPunctuator(":");

		Assets::LevelAdjoin adj;

		adj.Flags = FlagSet<Assets::SurfaceAdjoinFlag>(tok.GetNumber<uint32_t>());
		adj.Mirror = tok.GetNumber<size_t>();
		adj.Distance = tok.GetNumber<float>();

		lev.Adjoins.push_back(adj);
	}

	tok.AssertIdentifier("World");
	tok.AssertIdentifier("surfaces");

	size_t num_surfaces = tok.GetNumber<size_t>();

	for(size_t i = 0; i < num_surfaces; ++i) {
		lev.Surfaces.emplace_back();
		Assets::LevelSurface& surf = lev.Surfaces.back();

		tok.GetNumber<int>();
		tok.AssertPunctuator(":");

		surf.Material = tok.GetNumber<int>();
		surf.Flags = FlagSet<Assets::SurfaceFlag>(tok.GetNumber<uint32_t>());
		surf.FaceTypeFlags = FlagSet<Assets::FaceTypeFlag>(tok.GetNumber<uint32_t>());
		surf.GeometryMode = static_cast<Assets::GeometryMode>(tok.GetNumber<uint32_t>());
		surf.LightMode = static_cast<Assets::LightMode>(tok.GetNumber<uint32_t>());
		surf.TextureMode = static_cast<Assets::TextureMode>(tok.GetNumber<uint32_t>());
		surf.Adjoin = tok.GetNumber<int>();
		surf.ExtraLight = tok.GetNumber<float>();

		surf.Vertices.clear();

		size_t nverts = tok.GetNumber<size_t>();
		for(size_t i = 0; i < nverts; ++i) {
			int vx = tok.GetNumber<int>();
			tok.AssertPunctuator(",");
			int tx = tok.GetNumber<int>();

			surf.Vertices.emplace_back(vx, tx, 0.0f);
		}

		for(auto& vx : surf.Vertices) {
			std::get<2>(vx) = tok.GetNumber<float>();
		}
	}

	for(auto& surf : lev.Surfaces) {
		tok.GetNumber<int>();
		tok.AssertPunctuator(":");

		float x = tok.GetNumber<float>();
		float y = tok.GetNumber<float>();
		float z = tok.GetNumber<float>();

		surf.Normal = Math::Vec(x, y, z);
	}
}

void ParseSectorsSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	Text::Token t;

	tok.AssertIdentifier("World");
	tok.AssertIdentifier("sectors");

	size_t num_sectors = tok.GetNumber<size_t>();

	for(size_t i = 0; i < num_sectors; ++i) {
		Assets::LevelSector sec;

		while(true) {
			tok.GetToken(t);

			if(t.Type == Text::TokenType::EndOfFile) {
				report.AddError("LevelLoader::ParseSectorsSection", "unexpected end of file in sector", t.Location);
				return;
			}
			if(boost::iequals(t.Value, "sector")) {
				sec.Number = tok.GetNumber<size_t>();
			}
			else if(boost::iequals(t.Value, "flags")) {
				sec.Flags = FlagSet<Assets::SectorFlag>(tok.GetNumber<uint32_t>());
			}
			else if(boost::iequals(t.Value, "ambient")) {
				tok.AssertIdentifier("light");
				sec.AmbientLight = tok.GetNumber<float>();
			}
			else if(boost::iequals(t.Value, "extra")) {
				tok.AssertIdentifier("light");
				sec.ExtraLight = tok.GetNumber<float>();
			}
			else if(boost::iequals(t.Value, "colormap")) {
				sec.ColorMap = tok.GetNumber<size_t>();
			}
			else if(boost::iequals(t.Value, "tint")) {
				float r = tok.GetNumber<float>();
				float g = tok.GetNumber<float>();
				float b = tok.GetNumber<float>();
				sec.Tint = Math::Vec(r, g, b);
			}
			else if(boost::iequals(t.Value, "boundbox")) {
				float x0 = tok.GetNumber<float>();
				float y0 = tok.GetNumber<float>();
				float z0 = tok.GetNumber<float>();
				float x1 = tok.GetNumber<float>();
				float y1 = tok.GetNumber<float>();
				float z1 = tok.GetNumber<float>();

				sec.BoundingBox = Math::Box<3>(Math::Vec(x0, y0, z0), Math::Vec(x1, y1, z1));
			}
			else if(boost::iequals(t.Value, "collidebox")) {
				float x0 = tok.GetNumber<float>();
				float y0 = tok.GetNumber<float>();
				float z0 = tok.GetNumber<float>();
				float x1 = tok.GetNumber<float>();
				float y1 = tok.GetNumber<float>();
				float z1 = tok.GetNumber<float>();

				sec.CollideBox = Math::Box<3>(Math::Vec(x0, y0, z0), Math::Vec(x1, y1, z1));
			}
			else if(boost::iequals(t.Value, "sound")) {
				sec.AmbientSound = &manager.Load<Assets::Sound>(tok.GetSpaceDelimitedString());
				sec.AmbientSoundVolume = tok.GetNumber<float>();
			}
			else if(boost::iequals(t.Value, "center")) {
				float x = tok.GetNumber<float>();
				float y = tok.GetNumber<float>();
				float z = tok.GetNumber<float>();
				sec.Center = Math::Vec(x, y, z);
			}
			else if(boost::iequals(t.Value, "radius")) {
				sec.Radius = tok.GetNumber<float>();
			}
			else if(boost::iequals(t.Value, "vertices")) {
				size_t num_vertices = tok.GetNumber<size_t>();
				for(size_t i = 0; i < num_vertices; ++i) {
					tok.GetNumber<int>();
					tok.AssertPunctuator(":");
					sec.Vertices.push_back(tok.GetNumber<int>());
				}
			}
			else if(boost::iequals(t.Value, "surfaces")) {
				sec.FirstSurface = tok.GetNumber<size_t>();
				sec.SurfaceCount = tok.GetNumber<size_t>();
				break;
			}
			else {
				tok.SkipToNextLine();
				report.AddWarning("LevelLoader::ParseSectorsSection",
						boost::str(boost::format("unhandled sector field \'%s\'") % t.Value),
						t.Location);
			}
		}

		lev.Sectors.push_back(sec);
	}
}

void ParseCogsSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	tok.AssertIdentifier("World");
	tok.AssertIdentifier("Cogs");

	size_t num = tok.GetNumber<size_t>();

	Text::Token t;
	while(true) {
		tok.GetToken(t);

		if(t.Type == Text::TokenType::Identifier && boost::iequals(t.Value, "end")) {
			break;
		}
		else {
			tok.AssertPunctuator(":");

			Assets::Script const* script = nullptr;
			try {
				script = &manager.Load<Assets::Script>(tok.GetSpaceDelimitedString(), compiler);
			}
			catch(...) {
				// Failed to load script. Advance to next entry.
				tok.SkipToNextLine();
				continue;
			}

			std::vector<Cog::VM::Value> values;

			tok.SetReportEOL(true);

			for(const Cog::Symbols::Symbol& symbol : script->Script.SymbolTable) {
				if(!symbol.Local) {
					switch(symbol.Type) {
					case Cog::Symbols::SymbolType::Ai:
					case Cog::Symbols::SymbolType::Keyframe:
					case Cog::Symbols::SymbolType::Cog:
					case Cog::Symbols::SymbolType::Material:
					case Cog::Symbols::SymbolType::Model:
					case Cog::Symbols::SymbolType::Sound:
					case Cog::Symbols::SymbolType::Template:
					case Cog::Symbols::SymbolType::String:
						lev.CogStrings.emplace_back(new std::string(tok.GetSpaceDelimitedString()));
						values.push_back(lev.CogStrings.back()->data());
						break;

					case Cog::Symbols::SymbolType::Sector:
					case Cog::Symbols::SymbolType::Surface:
					case Cog::Symbols::SymbolType::Thing:
					case Cog::Symbols::SymbolType::Int:
						values.push_back(tok.GetNumber<int>());
						break;

					case Cog::Symbols::SymbolType::Flex:
					case Cog::Symbols::SymbolType::Float:
						values.push_back(tok.GetNumber<float>());
						break;

					case Cog::Symbols::SymbolType::Vector: {
						tok.AssertPunctuator("(");
						float x = tok.GetNumber<float>();
						tok.AssertPunctuator("/");
						float y = tok.GetNumber<float>();
						tok.AssertPunctuator("/");
						float z = tok.GetNumber<float>();
						tok.AssertPunctuator(")");
						values.push_back(Math::Vec(x, y, z));
					}
					break;

					case Cog::Symbols::SymbolType::Message:
						// Ignore
						break;

					default:
						report.AddWarning("LevelLoader::ParseCogsSection", "unhandled symbol type", t.Location);
						continue;
					}
				}
			}

			tok.SetReportEOL(false);

			lev.Cogs.emplace_back(script, values);
		}
	}

	if(num != lev.Cogs.size()) {
		report.AddWarning("LevelLoader::ParseCogsSection",
				boost::str(boost::format("expected %d cog instances, found %d entries") % num % lev.Cogs.size()),
				t.Location);
	}
}

void ParseTemplatesSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	tok.AssertIdentifier("World");
	tok.AssertIdentifier("templates");

	// Add 'none' template to list with default parameters.
	lev.TemplateMap.insert(std::make_pair("none", lev.Templates.size()));
	lev.Templates.push_back(Assets::Template());

	size_t num = tok.GetNumber<size_t>();

	std::string tpl_name;
	std::string base_name;
	while(true) {
		tpl_name = tok.GetSpaceDelimitedString();
		std::transform(tpl_name.begin(), tpl_name.end(), tpl_name.begin(), tolower);

		if(tpl_name == "end") {
			break;
		}
		else if(!tpl_name.empty()) {
			base_name = tok.GetSpaceDelimitedString();
			std::transform(base_name.begin(), base_name.end(), base_name.begin(), tolower);

			auto base_it = lev.TemplateMap.find(base_name);
			if(base_it == lev.TemplateMap.end()) {
				report.AddError("LevelLoader::ParseTemplatesSection",
						boost::str(boost::format("template %s parent %s not defined") % tpl_name % base_name), tok.GetInternalTokenLocation());
				tok.SkipToNextLine();
				continue;
			}

			auto succ_pair = lev.TemplateMap.emplace(tpl_name, lev.Templates.size());
			if(!succ_pair.second) {
				report.AddWarning("LevelLoader::ParseTemplatesSection",
						boost::str(boost::format("template %s redefinition") % tpl_name), tok.GetInternalTokenLocation());
			}

			lev.Templates.push_back(lev.Templates[base_it->second]);
			lev.Templates.back().ParseArgs(tok, manager, *lev.MasterColormap, compiler, report);
		}
		else {
			report.AddError("LevelLoader::ParseTemplatesSection", "expected template name", tok.GetInternalTokenLocation());
			break;
		}
	}

	if(num != lev.Templates.size()) {
		report.AddWarning("LevelLoader::ParseTemplatesSection",
				boost::str(boost::format("expected %d templates, found %d entries") % num % lev.Templates.size()),
				tok.GetInternalTokenLocation());
	}
}

void ParseThingsSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	tok.AssertIdentifier("World");
	tok.AssertIdentifier("things");

	size_t num = tok.GetNumber<size_t>();

	std::string tpl_name, thing_name;
	Text::Token t;
	while(true) {
		tok.GetToken(t);

		if(t.Type == Text::TokenType::Identifier && boost::iequals(t.Value, "end")) {
			break;
		}
		else {
			tok.AssertPunctuator(":");

			tpl_name = tok.GetSpaceDelimitedString();
			thing_name = tok.GetSpaceDelimitedString();

			float x = tok.GetNumber<float>();
			float y = tok.GetNumber<float>();
			float z = tok.GetNumber<float>();
			float pitch = tok.GetNumber<float>();
			float yaw = tok.GetNumber<float>();
			float roll = tok.GetNumber<float>();
			unsigned int sector = tok.GetNumber<unsigned int>();

			auto base_it = lev.TemplateMap.find(tpl_name);
			if(base_it == lev.TemplateMap.end()) {
				report.AddError("LevelLoader::ParseThingsSection",
						boost::str(boost::format("thing uses undefined template \'%s\'") % tpl_name),
						tok.GetInternalTokenLocation());
				tok.SkipToNextLine();
				continue;
			}

			lev.Things.emplace_back(lev.Templates[base_it->second]);
			lev.Things.back().Sector = sector;
			lev.Things.back().Position = Math::Vec(x, y, z);
			lev.Things.back().Orientation = Math::Vec(pitch, yaw, roll);
			lev.Things.back().ParseArgs(tok, manager, *lev.MasterColormap, compiler, report);
		}
	}

	if(num != lev.Things.size()) {
		report.AddWarning("LevelLoader::ParseThingsSection",
				boost::str(boost::format("expected %d things, found %d entries") % num % lev.Things.size()),
				t.Location);
	}
}

using LevelSectionParser = std::function<void(Assets::Level&, Text::Tokenizer&, Manager&, Cog::Compiler&, Diagnostics::Report&)>;
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

void PostprocessLevel(Assets::Level& lev, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	// Post-process; load materials and scripts.
	for(auto& mat_entry : lev.Materials) {
		std::get<0>(mat_entry) = &manager.Load<Assets::Material>(std::get<3>(mat_entry), *lev.MasterColormap);
	}

	// Add adjoined sector reference to adjoins.
	for(auto& surf : lev.Surfaces) {
		if(surf.Adjoin >= 0) {
			int mirror_adj = lev.Adjoins[surf.Adjoin].Mirror;
			for(const auto& sec : lev.Sectors) {
				bool found = false;
				for(size_t i = sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
					if(lev.Surfaces[i].Adjoin == mirror_adj) {
						found = true;
						surf.AdjoinedSector = sec.Number;
						break;
					}
				}

				if(found) {
					break;
				}
			}
		}
	}

	// Construct surface collide shapes.
	size_t i = 0;
	for(auto& surf : lev.Surfaces) {
		lev.SurfaceShapes.emplace_back(new btConvexHullShape());

		for(const auto& pt : surf.Vertices) {
			lev.SurfaceShapes.back()->addPoint(Math::BtVec(lev.Vertices[std::get<0>(pt)]));
		}

		lev.SurfaceShapes.back()->setMargin(0);
	}

	// Calculate axis-aligned bounding box for each sector;
	for(auto& sec : lev.Sectors) {
		Math::Vector<3> min_aabb = Math::Zero<3>(std::numeric_limits<float>::max());
		Math::Vector<3> max_aabb = Math::Zero<3>(std::numeric_limits<float>::lowest());

		for(auto& vx_id : sec.Vertices) {
			auto& vx = lev.Vertices[vx_id];

			for(auto vx_it = vx.begin(), min_it = min_aabb.begin(), max_it = max_aabb.begin();
					vx_it != vx.end(); ++vx_it, ++min_it, ++max_it) {
				*min_it = std::min(*min_it, *vx_it);
				*max_it = std::max(*max_it, *vx_it);
			}
		}

		sec.BoundingBox = Math::Box<3>(min_aabb, max_aabb);
	}
}

}
}
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::LevelLoader::Parse(Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Assets::Level> lev(new Assets::Level());

	Text::Token t;
	while(true) {
		SkipToNextSection(tok);
		tok.GetToken(t);

		if(t.Type == Text::TokenType::EndOfFile) {
			break;
		}
		else {
			std::transform(t.Value.begin(), t.Value.end(), t.Value.begin(), tolower);
			auto it = LevelSectionParserMap.find(t.Value);
			if(it == LevelSectionParserMap.end()) {
				report.AddWarning("LevelLoader", boost::str(boost::format("skipping unknown section %s") % t.Value), t.Location);
			}
			else {
				it->second(*lev, tok, manager, compiler, report);
			}
		}
	}

	PostprocessLevel(*lev, manager, compiler, report);

	return std::unique_ptr<Asset>(std::move(lev));
}

Gorc::Content::Loaders::LevelLoader::LevelLoader(Cog::Compiler& compiler)
	: compiler(compiler) {
	return;
}
