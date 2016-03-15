#include "log/log.hpp"
#include "model_loader.hpp"
#include "libold/content/assets/model.hpp"
#include "libold/base/content/content_manager.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>
#include "libold/content/constants.hpp"

gorc::fourcc const gorc::content::loaders::model_loader::type = "3DO"_4CC;

namespace {
    const std::vector<gorc::path> asset_root_path = { "3do" };
}

std::vector<gorc::path> const& gorc::content::loaders::model_loader::get_prefixes() const
{
    return asset_root_path;
}

namespace gorc {
namespace content {
namespace loaders {

void SkipToNextModelSection(text::tokenizer& tok) {
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

void ParseModelHeaderSection(assets::model&, text::tokenizer& tok, content_manager&) {
    std::string magic = tok.get_space_delimited_string();
    if(!boost::iequals(magic, "3DO")) {
        LOG_FATAL("file is not a valid 3DO");
    }

    tok.get_number<double>();
}

void ParseModelResourceSection(assets::model& model, text::tokenizer& tok, content_manager&) {
    tok.assert_identifier("materials");

    unsigned int num = tok.get_number<unsigned int>();
    for(unsigned int i = 0; i < num; ++i) {
        tok.get_number<unsigned int>();
        tok.assert_punctuator(":");
        model.material_entries.push_back(tok.get_space_delimited_string());
    }
}

void ParseGeometryDefSection(assets::model& model, text::tokenizer& tok, content_manager&) {
    tok.assert_identifier("RADIUS");
    model.radius = tok.get_number<float>();

    tok.assert_identifier("INSERT");
    tok.assert_identifier("OFFSET");
    float insert_x = tok.get_number<float>();
    float insert_y = tok.get_number<float>();
    float insert_z = tok.get_number<float>();
    model.insert_offset = make_vector(insert_x, insert_y, insert_z);

    tok.assert_identifier("GEOSETS");
    unsigned int num_geosets = tok.get_number<unsigned int>();

    for(unsigned int i = 0; i < num_geosets; ++i) {
        tok.assert_identifier("GEOSET");
        tok.get_number<int>();

        model.geosets.push_back(assets::model_geoset());
        assets::model_geoset& geoset = model.geosets.back();

        tok.assert_identifier("MESHES");
        unsigned int num_meshes = tok.get_number<unsigned int>();

        for(unsigned int j = 0; j < num_meshes; ++j) {
            geoset.meshes.push_back(assets::model_mesh());
            assets::model_mesh& mesh = geoset.meshes.back();

            tok.assert_identifier("MESH");
            mesh.index = tok.get_number<int>();

            tok.assert_identifier("NAME");
            // HACK: Some assets have spaces in mesh names. Don't really need it anyway.
            //mesh.Name = tok.get_space_delimited_string();
            tok.skip_to_next_line();

            tok.assert_identifier("RADIUS");
            mesh.radius = tok.get_number<float>();

            tok.assert_identifier("GEOMETRYMODE");
            mesh.geo = static_cast<flags::geometry_mode>(tok.get_number<uint32_t>());

            tok.assert_identifier("LIGHTINGMODE");
            mesh.light = static_cast<flags::light_mode>(tok.get_number<uint32_t>());

            tok.assert_identifier("TEXTUREMODE");
            mesh.tex = static_cast<flags::texture_mode>(tok.get_number<uint32_t>());

            tok.assert_identifier("VERTICES");
            unsigned int num_vertices = tok.get_number<unsigned int>();
            for(unsigned int k = 0; k < num_vertices; ++k) {
                tok.get_number<int>();
                tok.assert_punctuator(":");

                float v_x = tok.get_number<float>();
                float v_y = tok.get_number<float>();
                float v_z = tok.get_number<float>();
                tok.get_number<float>();

                mesh.vertices.push_back(make_vector(v_x, v_y, v_z));
            }

            tok.assert_identifier("TEXTURE");
            tok.assert_identifier("VERTICES");
            unsigned int num_tex_vertices = tok.get_number<unsigned int>();
            for(unsigned int k = 0; k < num_tex_vertices; ++k) {
                tok.get_number<int>();
                tok.assert_punctuator(":");

                float v_u = tok.get_number<float>();
                float v_v = tok.get_number<float>();

                mesh.texture_vertices.push_back(make_vector(v_u, v_v));
            }

            tok.assert_identifier("VERTEX");
            tok.assert_identifier("NORMALS");
            for(unsigned int k = 0; k < num_vertices; ++k) {
                tok.get_number<int>();
                tok.assert_punctuator(":");

                float n_x = tok.get_number<float>();
                float n_y = tok.get_number<float>();
                float n_z = tok.get_number<float>();

                mesh.vertex_normals.push_back(make_vector(n_x, n_y, n_z));
            }

            tok.assert_identifier("FACES");
            unsigned int num_faces = tok.get_number<unsigned int>();
            for(unsigned int k = 0; k < num_faces; ++k) {
                mesh.faces.push_back(assets::model_face());
                assets::model_face& face = mesh.faces.back();

                tok.get_number<int>();
                tok.assert_punctuator(":");

                face.material = tok.get_number<int>();
                face.type = flag_set<flags::face_flag>(tok.get_number<uint32_t>());
                face.geo = static_cast<flags::geometry_mode>(tok.get_number<uint32_t>());
                face.light = static_cast<flags::light_mode>(tok.get_number<uint32_t>());
                face.tex = static_cast<flags::texture_mode>(tok.get_number<uint32_t>());
                face.extra_light = tok.get_number<float>();

                unsigned int num_face_verts = tok.get_number<unsigned int>();
                for(unsigned int l = 0; l < num_face_verts; ++l) {
                    unsigned int geo_v = tok.get_number<unsigned int>();
                    tok.assert_punctuator(",");
                    unsigned int tex_v = tok.get_number<unsigned int>();

                    face.vertices.emplace_back(geo_v, tex_v);
                }
            }

            tok.assert_identifier("FACE");
            tok.assert_identifier("NORMALS");
            for(unsigned int k = 0; k < num_faces; ++k) {
                tok.get_number<int>();
                tok.assert_punctuator(":");

                float n_x = tok.get_number<float>();
                float n_y = tok.get_number<float>();
                float n_z = tok.get_number<float>();

                mesh.faces[k].normal = make_vector(n_x, n_y, n_z);
            }
        }
    }
}

void ParseHierarchyDefSection(assets::model& model, text::tokenizer& tok, content_manager&) {
    tok.assert_identifier("hierarchy");
    tok.assert_identifier("nodes");

    int nodes = tok.get_number<int>();
    model.hierarchy_nodes.resize(nodes);

    for(int i = 0; i < nodes; ++i) {
        auto& node = model.hierarchy_nodes[i];

        tok.get_number<int>();
        tok.assert_punctuator(":");

        tok.get_number<int>();

        node.type = flag_set<flags::mesh_node_type>(tok.get_number<unsigned int>());
        node.mesh = tok.get_number<int>();
        node.parent = tok.get_number<int>();
        node.child = tok.get_number<int>();
        node.sibling = tok.get_number<int>();
        node.num_children = tok.get_number<int>();

        get<0>(node.offset) = tok.get_number<float>();
        get<1>(node.offset) = tok.get_number<float>();
        get<2>(node.offset) = tok.get_number<float>();

        get<0>(node.rotation) = tok.get_number<float>();
        get<1>(node.rotation) = tok.get_number<float>();
        get<2>(node.rotation) = tok.get_number<float>();

        get<0>(node.pivot) = tok.get_number<float>();
        get<1>(node.pivot) = tok.get_number<float>();
        get<2>(node.pivot) = tok.get_number<float>();

        node.name = tok.get_space_delimited_string();
    }
}

void PostprocessModel(assets::model& model, content_manager& manager)
{
    for(const auto& mat_name : model.material_entries) {
        model.materials.push_back(manager.load<assets::material>(mat_name));
    }

    // Some color faces have invalid texture coordinates.
    // Ensure each mesh has at least one texture vertex.
    for(auto& geoset : model.geosets) {
        for(auto& mesh : geoset.meshes) {
            if(mesh.texture_vertices.empty()) {
                mesh.texture_vertices.push_back(make_vector(0.0f, 0.0f));
            }
        }
    }

    return;
}

using ModelLoaderSectionFn = std::function<void(assets::model&, text::tokenizer&, content_manager&)>;
const std::unordered_map<std::string, ModelLoaderSectionFn> ModelLoaderSectionMap {
    { "header", ParseModelHeaderSection },
    { "modelresource", ParseModelResourceSection },
    { "geometrydef", ParseGeometryDefSection },
    { "hierarchydef", ParseHierarchyDefSection }
};

}
}
}

std::unique_ptr<gorc::asset> gorc::content::loaders::model_loader::parse(text::tokenizer& tok, content_manager& manager, service_registry const &) const {
    std::unique_ptr<assets::model> lev(new assets::model());

    text::token t;
    while(true) {
        SkipToNextModelSection(tok);
        tok.get_token(t);

        if(t.type == text::token_type::end_of_file) {
            break;
        }
        else {
            std::transform(t.value.begin(), t.value.end(), t.value.begin(), tolower);
            auto it = ModelLoaderSectionMap.find(t.value);
            if(it == ModelLoaderSectionMap.end()) {
                diagnostic_context dc(nullptr, t.location.first_line, t.location.last_line);
                LOG_WARNING(format("skipping unknown section %s") % t.value);
            }
            else {
                it->second(*lev, tok, manager);
            }
        }
    }

    PostprocessModel(*lev, manager);

    return std::unique_ptr<asset>(std::move(lev));
}
