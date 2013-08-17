#include "model_loader.h"
#include "content/assets/model.h"
#include "content/manager.h"
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>
#include "content/constants.h"

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::ModelLoader::AssetRootPath = { "3do" };

namespace Gorc {
namespace Content {
namespace Loaders {

void SkipToNextModelSection(Text::Tokenizer& tok) {
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

void ParseModelHeaderSection(Assets::Model& model, Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	std::string magic = tok.GetSpaceDelimitedString();
	if(!boost::iequals(magic, "3DO")) {
		report.AddError("ModelLoader::ParseModelHeaderSection", boost::str(boost::format("expected \'3DO\', found \'%s\'") % magic),
				tok.GetInternalTokenLocation());
		throw Text::TokenizerAssertionException();
	}

	tok.GetNumber<double>();
}

void ParseModelResourceSection(Assets::Model& model, Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	tok.AssertIdentifier("materials");

	unsigned int num = tok.GetNumber<unsigned int>();
	for(unsigned int i = 0; i < num; ++i) {
		tok.GetNumber<unsigned int>();
		tok.AssertPunctuator(":");
		model.MaterialEntries.push_back(tok.GetSpaceDelimitedString());
	}
}

void ParseGeometryDefSection(Assets::Model& model, Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	tok.AssertIdentifier("RADIUS");
	model.Radius = tok.GetNumber<float>();

	tok.AssertIdentifier("INSERT");
	tok.AssertIdentifier("OFFSET");
	float insert_x = tok.GetNumber<float>();
	float insert_y = tok.GetNumber<float>();
	float insert_z = tok.GetNumber<float>();
	model.InsertOffset = Math::Vec(insert_x, insert_y, insert_z);

	tok.AssertIdentifier("GEOSETS");
	unsigned int num_geosets = tok.GetNumber<unsigned int>();

	for(unsigned int i = 0; i < num_geosets; ++i) {
		tok.AssertIdentifier("GEOSET");
		tok.GetNumber<int>();

		model.GeoSets.push_back(Assets::ModelGeoSet());
		Assets::ModelGeoSet& geoset = model.GeoSets.back();

		tok.AssertIdentifier("MESHES");
		unsigned int num_meshes = tok.GetNumber<unsigned int>();

		for(unsigned int j = 0; j < num_meshes; ++j) {
			geoset.Meshes.push_back(Assets::ModelMesh());
			Assets::ModelMesh& mesh = geoset.Meshes.back();

			tok.AssertIdentifier("MESH");
			mesh.Index = tok.GetNumber<int>();

			tok.AssertIdentifier("NAME");
			// HACK: Some assets have spaces in mesh names. Don't really need it anyway.
			//mesh.Name = tok.GetSpaceDelimitedString();
			tok.SkipToNextLine();

			tok.AssertIdentifier("RADIUS");
			mesh.Radius = tok.GetNumber<float>();

			tok.AssertIdentifier("GEOMETRYMODE");
			mesh.Geo = static_cast<Flags::GeometryMode>(tok.GetNumber<uint32_t>());

			tok.AssertIdentifier("LIGHTINGMODE");
			mesh.Light = static_cast<Flags::LightMode>(tok.GetNumber<uint32_t>());

			tok.AssertIdentifier("TEXTUREMODE");
			mesh.Tex = static_cast<Flags::TextureMode>(tok.GetNumber<uint32_t>());

			tok.AssertIdentifier("VERTICES");
			unsigned int num_vertices = tok.GetNumber<unsigned int>();
			for(unsigned int k = 0; k < num_vertices; ++k) {
				tok.GetNumber<int>();
				tok.AssertPunctuator(":");

				float v_x = tok.GetNumber<float>();
				float v_y = tok.GetNumber<float>();
				float v_z = tok.GetNumber<float>();
				tok.GetNumber<float>();

				mesh.Vertices.push_back(Math::Vec(v_x, v_y, v_z));
			}

			tok.AssertIdentifier("TEXTURE");
			tok.AssertIdentifier("VERTICES");
			unsigned int num_tex_vertices = tok.GetNumber<unsigned int>();
			for(unsigned int k = 0; k < num_tex_vertices; ++k) {
				tok.GetNumber<int>();
				tok.AssertPunctuator(":");

				float v_u = tok.GetNumber<float>();
				float v_v = tok.GetNumber<float>();

				mesh.TextureVertices.push_back(Math::Vec(v_u, v_v));
			}

			tok.AssertIdentifier("VERTEX");
			tok.AssertIdentifier("NORMALS");
			for(unsigned int k = 0; k < num_vertices; ++k) {
				tok.GetNumber<int>();
				tok.AssertPunctuator(":");

				float n_x = tok.GetNumber<float>();
				float n_y = tok.GetNumber<float>();
				float n_z = tok.GetNumber<float>();

				mesh.VertexNormals.push_back(Math::Vec(n_x, n_y, n_z));
			}

			tok.AssertIdentifier("FACES");
			unsigned int num_faces = tok.GetNumber<unsigned int>();
			for(unsigned int k = 0; k < num_faces; ++k) {
				mesh.Faces.push_back(Assets::ModelFace());
				Assets::ModelFace& face = mesh.Faces.back();

				tok.GetNumber<int>();
				tok.AssertPunctuator(":");

				face.Material = tok.GetNumber<int>();
				face.Type = FlagSet<Flags::FaceFlag>(tok.GetNumber<uint32_t>());
				face.Geo = static_cast<Flags::GeometryMode>(tok.GetNumber<uint32_t>());
				face.Light = static_cast<Flags::LightMode>(tok.GetNumber<uint32_t>());
				face.Tex = static_cast<Flags::TextureMode>(tok.GetNumber<uint32_t>());
				face.ExtraLight = tok.GetNumber<float>();

				unsigned int num_face_verts = tok.GetNumber<unsigned int>();
				for(unsigned int l = 0; l < num_face_verts; ++l) {
					unsigned int geo_v = tok.GetNumber<unsigned int>();
					tok.AssertPunctuator(",");
					unsigned int tex_v = tok.GetNumber<unsigned int>();

					face.Vertices.emplace_back(geo_v, tex_v);
				}
			}

			tok.AssertIdentifier("FACE");
			tok.AssertIdentifier("NORMALS");
			for(unsigned int k = 0; k < num_faces; ++k) {
				tok.GetNumber<int>();
				tok.AssertPunctuator(":");

				float n_x = tok.GetNumber<float>();
				float n_y = tok.GetNumber<float>();
				float n_z = tok.GetNumber<float>();

				mesh.Faces[k].Normal = Math::Vec(n_x, n_y, n_z);
			}
		}
	}
}

void ParseHierarchyDefSection(Assets::Model& model, Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	tok.AssertIdentifier("hierarchy");
	tok.AssertIdentifier("nodes");

	int nodes = tok.GetNumber<int>();
	model.HierarchyNodes.resize(nodes);

	for(int i = 0; i < nodes; ++i) {
		auto& node = model.HierarchyNodes[i];

		tok.GetNumber<int>();
		tok.AssertPunctuator(":");

		tok.GetNumber<int>();

		node.Type = FlagSet<Flags::MeshNodeType>(tok.GetNumber<unsigned int>());
		node.Mesh = tok.GetNumber<int>();
		node.Parent = tok.GetNumber<int>();
		node.Child = tok.GetNumber<int>();
		node.Sibling = tok.GetNumber<int>();
		node.NumChildren = tok.GetNumber<int>();

		Math::Get<0>(node.Offset) = tok.GetNumber<float>();
		Math::Get<1>(node.Offset) = tok.GetNumber<float>();
		Math::Get<2>(node.Offset) = tok.GetNumber<float>();

		Math::Get<0>(node.Rotation) = tok.GetNumber<float>();
		Math::Get<1>(node.Rotation) = tok.GetNumber<float>();
		Math::Get<2>(node.Rotation) = tok.GetNumber<float>();

		Math::Get<0>(node.Pivot) = tok.GetNumber<float>();
		Math::Get<1>(node.Pivot) = tok.GetNumber<float>();
		Math::Get<2>(node.Pivot) = tok.GetNumber<float>();

		node.Name = tok.GetSpaceDelimitedString();
	}
}

void PostprocessModel(Assets::Model& model, Manager& manager, const Assets::Colormap& colormap, Diagnostics::Report& report) {
	for(const auto& mat_name : model.MaterialEntries) {
		model.Materials.push_back(&manager.Load<Assets::Material>(mat_name, colormap));
	}

	// Some color faces have invalid texture coordinates.
	// Ensure each mesh has at least one texture vertex.
	for(auto& geoset : model.GeoSets) {
		for(auto& mesh : geoset.Meshes) {
			if(mesh.TextureVertices.empty()) {
				mesh.TextureVertices.push_back(Math::Vec(0.0f, 0.0f));
			}
		}
	}

	// Generate mesh shapes
	model.Shape = std::unique_ptr<btCompoundShape>(new btCompoundShape());
	for(auto& mesh : model.GeoSets[0].Meshes) {
		mesh.Shape = std::unique_ptr<btCompoundShape>(new btCompoundShape());

		for(auto& face : mesh.Faces) {
			face.Shape = std::unique_ptr<btConvexHullShape>(new btConvexHullShape());
			for(auto& vertex : face.Vertices) {
				face.Shape->addPoint(Math::BtVec(mesh.Vertices[std::get<0>(vertex)]) * PhysicsWorldScale);
			}

			face.Shape->setMargin(0);
			mesh.Shape->addChildShape(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)), face.Shape.get());
		}

		model.Shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 0.0f, 0.0f)), mesh.Shape.get());
	}

	return;
}

using ModelLoaderSectionFn = std::function<void(Assets::Model&, Text::Tokenizer&, Manager&, Diagnostics::Report&)>;
const std::unordered_map<std::string, ModelLoaderSectionFn> ModelLoaderSectionMap {
	{ "header", ParseModelHeaderSection },
	{ "modelresource", ParseModelResourceSection },
	{ "geometrydef", ParseGeometryDefSection },
	{ "hierarchydef", ParseHierarchyDefSection }
};

}
}
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::ModelLoader::Parse(Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Assets::Model> lev(new Assets::Model());

	Text::Token t;
	while(true) {
		SkipToNextModelSection(tok);
		tok.GetToken(t);

		if(t.Type == Text::TokenType::EndOfFile) {
			break;
		}
		else {
			std::transform(t.Value.begin(), t.Value.end(), t.Value.begin(), tolower);
			auto it = ModelLoaderSectionMap.find(t.Value);
			if(it == ModelLoaderSectionMap.end()) {
				report.AddWarning("ModelLoader", boost::str(boost::format("skipping unknown section %s") % t.Value), t.Location);
			}
			else {
				it->second(*lev, tok, manager, report);
			}
		}
	}

	PostprocessModel(*lev, manager, colormap, report);

	return std::unique_ptr<Asset>(std::move(lev));
}

Gorc::Content::Loaders::ModelLoader::ModelLoader(const Assets::Colormap& colormap)
	: colormap(colormap) {
	return;
}
