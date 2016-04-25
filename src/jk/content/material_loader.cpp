#include "material_loader.hpp"
#include "material.hpp"
#include "raw_material.hpp"
#include "renderer_object_factory.hpp"
#include "libold/content/master_colormap.hpp"
#include "math/color.hpp"

gorc::fourcc const gorc::material_loader::type = "MAT"_4CC;

namespace {
    std::vector<gorc::path> const asset_root_path = { "mat", "3do/mat" };
}

std::vector<gorc::path> const& gorc::material_loader::get_prefixes() const
{
    return asset_root_path;
}

gorc::maybe<char const *> gorc::material_loader::get_default() const
{
    return "dflt.mat";
}

namespace {
    using namespace gorc;

    constexpr int color_tex_dim = 8;

    class material_processor {
    public:
        virtual ~material_processor()
        {
            return;
        }

        virtual void process_color_cel(raw_material_cel_record const &cel,
                                       raw_material const &mat,
                                       material_id id,
                                       int cel_number) = 0;

        virtual void process_texture_cel(raw_material_texture_record const &tex,
                                         raw_material_cel_record const &cel,
                                         raw_material const &mat,
                                         material_id id,
                                         int cel_number) = 0;
    };

    class indexed_material_processor : public material_processor {
    public:
        asset_ref<colormap> cmp;
        renderer_object_factory &obj_factory;

        indexed_material_processor(asset_ref<colormap> cmp,
                                   renderer_object_factory &obj_factory)
            : cmp(cmp)
            , obj_factory(obj_factory)
        {
            return;
        }

        virtual void process_color_cel(raw_material_cel_record const &cel,
                                       raw_material const &,
                                       material_id id,
                                       int cel_number) override
        {
            grid<color_rgba8> col_img(make_size(color_tex_dim, color_tex_dim));
            grid<color_rgba8> lht_img(make_size(color_tex_dim, color_tex_dim));

            for(size_t y = 0; y < color_tex_dim; ++y) {
                for(size_t x = 0; x < color_tex_dim; ++x) {
                    col_img.get(x, y) = solid(cmp->get_color(cel.color_index));
                    lht_img.get(x, y) = solid(cmp->get_light_color(cel.color_index));
                }
            }

            obj_factory.set_material_image(id, cel_number, /* diffuse */ 0, col_img);
            obj_factory.set_material_image(id, cel_number, /* light */ 1, lht_img);
        }

        virtual void process_texture_cel(raw_material_texture_record const &tex,
                                         raw_material_cel_record const &cel,
                                         raw_material const &mat,
                                         material_id id,
                                         int cel_number) override
        {
            grid<color_rgba8> col_img(make_size(tex.width, tex.height));
            grid<color_rgba8> lht_img(make_size(tex.width, tex.height));

            auto it = tex.image_data.at(0).begin();
            auto end = tex.image_data.at(0).end();

            int transparent_idx = -1;
            if(tex.uses_transparency) {
                transparent_idx = mat.transparency;
            }

            for(size_t y = 0; y < tex.height; ++y) {
                for(size_t x = 0; x < tex.width; ++x) {
                    if(it == end) {
                        LOG_FATAL(format("not enough data for texture %d") % cel.texture_index);
                    }

                    uint8_t idx = *it++;
                    uint8_t alpha = 255;
                    if(static_cast<int>(idx) == transparent_idx) {
                        alpha = 0;
                    }

                    col_img.get(x, y) = make_color_rgba8(cmp->get_color(idx), alpha);
                    lht_img.get(x, y) = make_color_rgba8(cmp->get_light_color(idx), alpha);
                }
            }

            obj_factory.set_material_image(id, cel_number, /* diffuse */ 0, col_img);
            obj_factory.set_material_image(id, cel_number, /* light */ 1, lht_img);
        }
    };

}

std::unique_ptr<gorc::asset> gorc::material_loader::deserialize(input_stream &is,
                                                                content_manager &,
                                                                asset_id id,
                                                                service_registry const &svc) const
{
    auto mat = std::make_unique<material>();

    binary_input_stream bis(is);
    raw_material rm(deserialization_constructor, bis);

    material_id mid(static_cast<int>(id));

    std::unique_ptr<material_processor> proc;
    if(rm.bitdepth == 8) {
        proc = std::make_unique<indexed_material_processor>(
                svc.get<content::master_colormap>().cmp.get_value(),
                svc.get<renderer_object_factory>());
    }
    else {
        LOG_FATAL("unknown pixel format");
    }

    for(int cel = 0; cel < static_cast<int>(rm.cel_records.size()); ++cel) {
        auto const &cel_record = rm.cel_records.at(cel);
        if(cel_record.type == 0) {
            proc->process_color_cel(cel_record,
                                    rm,
                                    mid,
                                    cel);

            mat->cels.push_back(make_size(color_tex_dim, color_tex_dim));
        }
        else if(cel_record.type == 8) {
            auto const &tex_record = rm.texture_records.at(cel_record.texture_index);
            proc->process_texture_cel(tex_record,
                                      cel_record,
                                      rm,
                                      mid,
                                      cel);

            mat->cels.push_back(make_size(tex_record.width, tex_record.height));
        }
        else {
            LOG_FATAL(format("cel %d has unknown type %d") %
                      cel %
                      cel_record.type);
        }
    }

    return std::move(mat);
}
