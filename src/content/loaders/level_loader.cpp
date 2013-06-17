#include "level_loader.h"
#include "content/assets/level.h"
#include "content/manager.h"
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unordered_map>
#include <string>
#include <tuple>

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

			t.Value = tok.GetFilename();
			float x_scale = tok.GetNumber<float>();
			float y_scale = tok.GetNumber<float>();

			lev.MaterialEntries.emplace_back(t.Value, x_scale, y_scale);
		}
	}

	if(num != lev.MaterialEntries.size()) {
		report.AddWarning("LevelLoader::ParseMaterialsSection",
				boost::str(boost::format("expected %d materials, found %d entries") % num % lev.MaterialEntries.size()),
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

		lev.ColormapEntries.push_back(tok.GetFilename());
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
				sec.AmbientSound = &manager.Load<Assets::Sound>(tok.GetFilename());
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

void ParseCogscriptsSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	tok.AssertIdentifier("World");
	tok.AssertIdentifier("scripts");

	size_t num = tok.GetNumber<size_t>();

	Text::Token t;
	while(true) {
		tok.GetToken(t);

		if(t.Type == Text::TokenType::Identifier && boost::iequals(t.Value, "end")) {
			break;
		}
		else {
			tok.AssertPunctuator(":");

			t.Value = tok.GetFilename();

			lev.ScriptEntries.emplace_back(t.Value);
		}
	}

	if(num != lev.ScriptEntries.size()) {
		report.AddWarning("LevelLoader::ParseCogscriptsSection",
				boost::str(boost::format("expected %d scripts, found %d entries") % num % lev.ScriptEntries.size()),
				t.Location);
	}
}

void ParseCogsSection(Assets::Level& lev, Text::Tokenizer& tok, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {

}

void PostprocessLevel(Assets::Level& lev, Manager& manager, Cog::Compiler& compiler, Diagnostics::Report& report) {
	// Post-process; load colormaps, materials, scripts.
	for(const auto& cmp_entry : lev.ColormapEntries) {
		lev.Colormaps.push_back(&manager.Load<Assets::Colormap>(cmp_entry));
	}

	const Assets::Colormap& master_cmp = *lev.Colormaps[0];

	for(const auto& mat_entry : lev.MaterialEntries) {
		lev.Materials.push_back(&manager.Load<Assets::Material>(std::get<0>(mat_entry), master_cmp));
	}

	for(const auto& script_entry : lev.ScriptEntries) {
		lev.Scripts.push_back(&manager.Load<Assets::Script>(script_entry, compiler));
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

	// Construct level index and vertex buffers.
	for(const auto& surf : lev.Surfaces) {
		const auto& first_vx = std::get<0>(surf.Vertices[0]);
		for(size_t j = 2; j < surf.Vertices.size(); ++j) {
			lev.SurfaceIndexBuffer.push_back(first_vx);
			lev.SurfaceIndexBuffer.push_back(std::get<0>(surf.Vertices[j - 1]));
			lev.SurfaceIndexBuffer.push_back(std::get<0>(surf.Vertices[j]));
		}
	}

	// Construct surface collide shapes.
	size_t i = 0;
	for(auto& surf : lev.Surfaces) {
		surf.SurfaceIndexVertexArray = std::unique_ptr<btTriangleIndexVertexArray>(new btTriangleIndexVertexArray(
				surf.Vertices.size() - 2, &lev.SurfaceIndexBuffer[i], sizeof(int) * 3,
				lev.Vertices.size(), reinterpret_cast<float*>(&lev.Vertices[0]), sizeof(Math::Vector<3>)));
		surf.SurfaceShape = std::unique_ptr<btBvhTriangleMeshShape>(new btBvhTriangleMeshShape(
				surf.SurfaceIndexVertexArray.get(), true));

		i += (surf.Vertices.size() - 2) * 3;
	}
}

}
}
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::LevelLoader::Parse(Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Assets::Level> lev(new Assets::Level());

	using sec_fn = std::function<void(Assets::Level&, Text::Tokenizer&, Manager&, Cog::Compiler&, Diagnostics::Report&)>;
	std::vector<std::tuple<std::string, sec_fn>> sectionmap {
		std::make_tuple("jk", ParseJKSection),
		std::make_tuple("header", ParseHeaderSection),
		std::make_tuple("materials", ParseMaterialsSection),
		std::make_tuple("georesource", ParseGeoresourceSection),
		std::make_tuple("sectors", ParseSectorsSection),
		std::make_tuple("cogscripts", ParseCogscriptsSection)
	};

	Text::Token t;
	while(true) {
		SkipToNextSection(tok);
		tok.GetToken(t);

		if(t.Type == Text::TokenType::EndOfFile) {
			break;
		}
		else {
			auto it = std::find_if(sectionmap.begin(), sectionmap.end(),
					[&t](const std::tuple<std::string, sec_fn>& key){ return boost::iequals(t.Value, std::get<0>(key)); });
			if(it == sectionmap.end()) {
				report.AddWarning("LevelLoader", boost::str(boost::format("skipping unknown section %s") % t.Value), t.Location);
			}
			else {
				std::get<1>(*it)(*lev, tok, manager, compiler, report);
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
