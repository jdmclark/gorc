#include "libold/base/content/assets/shader.hpp"
#include "level_view.hpp"
#include "thing_mesh_node_visitor.hpp"
#include "pov_mesh_node_visitor.hpp"
#include "game/world/level_presenter.hpp"
#include "client/application.hpp"
#include "game/world/level_model.hpp"
#include "game/world/keys/key_presenter.hpp"
#include "game/world/scripts/script_presenter.hpp"
#include "libold/content/assets/model.hpp"
#include "libold/content/assets/sprite.hpp"
#include "libold/content/constants.hpp"
#include "game/world/components/thing.hpp"

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <GL/glu.h>

gorc::client::world::level_view::level_view(content::content_manager& contentmanager)
    : surfaceShader(contentmanager.load<content::assets::shader>("surface.glsl")),
      horizonShader(contentmanager.load<content::assets::shader>("horizon.glsl")),
      ceilingShader(contentmanager.load<content::assets::shader>("ceiling.glsl")),
      lightShader(contentmanager.load<content::assets::shader>("light.glsl")),
      currentPresenter(nullptr), currentModel(nullptr) {
    return;
}

void gorc::client::world::level_view::compute_visible_sectors(const box<2, int>&) {
    const auto& cam = currentModel->camera_model.current_computed_state;

    std::array<double, 16> proj_matrix;
    std::array<double, 16> view_matrix;
    std::array<int, 4> viewport;

    float const* proj = make_opengl_matrix(projection_matrix);
    std::transform(proj, proj + 16, proj_matrix.begin(), [](float value) { return static_cast<double>(value); });

    float const* view = make_opengl_matrix(this->view_matrix);
    std::transform(view, view + 16, view_matrix.begin(), [](float value) { return static_cast<double>(value); });

    glGetIntegerv(GL_VIEWPORT, viewport.data());

    box<2, double> adj_bbox(make_vector(static_cast<double>(std::get<0>(viewport)), static_cast<double>(std::get<1>(viewport))),
            make_vector(static_cast<double>(std::get<0>(viewport) + std::get<2>(viewport)),
                    static_cast<double>(std::get<1>(viewport) + std::get<3>(viewport))));

    sector_visited_scratch.clear();
    sector_vis_scratch.clear();
    sector_vis_scratch.emplace(cam.containing_sector);
    do_sector_vis(cam.containing_sector, proj_matrix, view_matrix, viewport, adj_bbox, cam.position, cam.look);
}

void gorc::client::world::level_view::do_sector_vis(unsigned int sec_num, const std::array<double, 16>& proj_mat, const std::array<double, 16>& view_mat,
        const std::array<int, 4>& viewport, const box<2, double>& adj_bbox, const vector<3>& cam_pos, const vector<3>& cam_look) {
    sector_visited_scratch.emplace(sec_num);

    const auto& sector = currentModel->sectors[sec_num];
    for(int i = 0; i < sector.surface_count; ++i) {
        const auto& surface = currentModel->surfaces[sector.first_surface + i];

        const vector<3>& surf_vx_pos = currentModel->level->vertices[std::get<0>(surface.vertices.front())];

        if(surface.adjoin < 0 || sector_visited_scratch.count(surface.adjoined_sector) > 0) {
            continue;
        }

        const auto& adjoin = currentModel->adjoins[surface.adjoin];
        if(!(adjoin.flags & flags::adjoin_flag::Visible)) {
            continue;
        }

        float dist = dot(surface.normal, cam_pos - surf_vx_pos);
        if(dist < 0.0f) {
            continue;
        }

        double min_x = std::numeric_limits<double>::max();
        double min_y = std::numeric_limits<double>::max();
        double max_x = std::numeric_limits<double>::lowest();
        double max_y = std::numeric_limits<double>::lowest();

        bool failed = false;
        bool culled = true;
        for(const auto& vx : surface.vertices) {
            auto vx_pos = currentModel->level->vertices[std::get<0>(vx)];

            if(dot(cam_look, vx_pos - cam_pos) < 0.0f) {
                // Vertex behind camera.
                failed = true;
                continue;
            }

            culled = false;

            double x_out, y_out, z_out;

            gluProject(get<0>(vx_pos), get<1>(vx_pos), get<2>(vx_pos),
                    view_mat.data(), proj_mat.data(), viewport.data(),
                    &x_out, &y_out, &z_out);

            min_x = std::min(min_x, x_out);
            max_x = std::max(max_x, x_out);
            min_y = std::min(min_y, y_out);
            max_y = std::max(max_y, y_out);
        }

        if(culled) {
            continue;
        }

        if(failed) {
            sector_vis_scratch.emplace(surface.adjoined_sector);
            do_sector_vis(surface.adjoined_sector, proj_mat, view_mat, viewport, adj_bbox, cam_pos, cam_look);
            continue;
        }

        box<2, double> new_adj_bbox(make_vector(min_x, min_y), make_vector(max_x, max_y));
        if(adj_bbox.overlaps(new_adj_bbox)) {
            sector_vis_scratch.emplace(surface.adjoined_sector);
            do_sector_vis(surface.adjoined_sector, proj_mat, view_mat, viewport,
                    adj_bbox & new_adj_bbox, cam_pos, cam_look);
        }
    }

    sector_visited_scratch.erase(sec_num);
}

void gorc::client::world::level_view::record_visible_special_surfaces() {
    for(auto sec_num : sector_vis_scratch) {
        const content::assets::level_sector& sector = currentModel->sectors[sec_num];

        for(int i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
            const auto& surface = currentModel->surfaces[i];

            if(surface.geometry_mode == flags::geometry_mode::not_drawn) {
                continue;
            }

            if(surface.adjoin < 0 && (surface.flags & flags::surface_flag::HorizonSky)) {
                horizon_sky_surfaces_scratch.push_back(std::make_tuple(sec_num, i));
            }
            else if(surface.adjoin < 0 && (surface.flags & flags::surface_flag::CeilingSky)) {
                ceiling_sky_surfaces_scratch.push_back(std::make_tuple(sec_num, i));
            }
            else if(surface.adjoin >= 0) {
                translucent_surfaces_scratch.push_back(std::make_tuple(sec_num, i, 0.0f));
            }
        }
    }

    const auto& cam_pos = currentModel->camera_model.current_computed_state.position;

    // Compute distances to translucent surfaces
    for(auto& surf_tuple : translucent_surfaces_scratch) {
        unsigned int surf_id = std::get<1>(surf_tuple);
        const auto& surf = currentModel->surfaces[surf_id];
        auto vx_pos = currentModel->level->vertices[std::get<0>(surf.vertices.front())];
        std::get<2>(surf_tuple) = dot(surf.normal, cam_pos - vx_pos);

        // TODO: Currently uses distance to plane. Compute actual distance to polygon.
    }

    // Sort translucent surfaces back to front.
    std::sort(translucent_surfaces_scratch.begin(), translucent_surfaces_scratch.end(),
            [](const std::tuple<unsigned int, unsigned int, float>& a, const std::tuple<unsigned int, unsigned int, float>& b) {
        return std::get<2>(a) > std::get<2>(b);
    });
}

void gorc::client::world::level_view::record_visible_things() {
    for(auto& thing_pair : currentModel->ecs.all_components<gorc::game::world::components::thing>()) {
        if(sector_vis_scratch.find(thing_pair.second.sector) != sector_vis_scratch.end()) {
            visible_thing_scratch.emplace_back(static_cast<int>(thing_pair.first), length(thing_pair.second.position - currentModel->camera_model.current_computed_state.position));

            if(!(thing_pair.second.flags & flags::thing_flag::Sighted)) {
                // thing has been sighted for first time. Fire sighted event.
                currentPresenter->thing_sighted(static_cast<int>(thing_pair.first));
            }
        }
    }

    // Sort things from back to front.
    std::sort(visible_thing_scratch.begin(), visible_thing_scratch.end(),
            [](const std::tuple<int, float>& a, const std::tuple<int, float>& b) {
        return std::get<1>(a) > std::get<1>(b);
    });
}

void gorc::client::world::level_view::draw_visible_diffuse_surfaces() {
    glDepthMask(GL_TRUE);
    for(auto sec_num : sector_vis_scratch) {
        const content::assets::level_sector& sector = currentModel->sectors[sec_num];

        for(int i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
            const auto& surface = currentModel->surfaces[i];

            if(surface.geometry_mode == flags::geometry_mode::not_drawn
                    || surface.adjoin >= 0
                    || (surface.flags & flags::surface_flag::HorizonSky)
                    || (surface.flags & flags::surface_flag::CeilingSky)) {
                continue;
            }

            draw_surface(i, sector, 1.0f);
        }
    }
}

void gorc::client::world::level_view::draw_visible_sky_surfaces(const box<2, int>& view_size, const vector<3>& sector_tint) {
    glDepthMask(GL_TRUE);

    if(!horizon_sky_surfaces_scratch.empty()) {
        set_current_shader(horizonShader, sector_tint,
                currentModel->header.horizon_sky_offset, currentModel->header.horizon_pixels_per_rev,
                currentModel->header.horizon_distance, view_size, currentModel->camera_model.current_computed_state.look);

        for(auto surf_tuple : horizon_sky_surfaces_scratch) {
            draw_surface(std::get<1>(surf_tuple), currentModel->sectors[std::get<0>(surf_tuple)], 1.0f);
        }
    }

    if(!ceiling_sky_surfaces_scratch.empty()) {
        set_current_shader(ceilingShader, sector_tint,
                currentModel->header.ceiling_sky_offset, currentModel->header.ceiling_sky_z);

        for(auto surf_tuple : ceiling_sky_surfaces_scratch) {
            draw_surface(std::get<1>(surf_tuple), currentModel->sectors[std::get<0>(surf_tuple)], 1.0f);
        }
    }
}

void gorc::client::world::level_view::draw_visible_translucent_surfaces_and_things() {
    auto thing_it = visible_thing_scratch.begin();
    auto surf_it = translucent_surfaces_scratch.begin();
    while(thing_it != visible_thing_scratch.end() && surf_it != translucent_surfaces_scratch.end()) {
        if(std::get<1>(*thing_it) <= std::get<2>(*surf_it)) {
            glDepthMask(GL_TRUE);
            glDisable(GL_CULL_FACE);
            draw_thing(currentModel->get_thing(std::get<0>(*thing_it)), std::get<0>(*thing_it));
            ++thing_it;
        }
        else {
            glDepthMask(GL_FALSE);
            glEnable(GL_CULL_FACE);
            draw_surface(std::get<1>(*surf_it), currentModel->sectors[std::get<0>(*surf_it)],
                    (currentModel->surfaces[std::get<1>(*surf_it)].face_type_flags & flags::face_flag::Translucent) ? 0.5f : 1.0f);
            ++surf_it;
        }
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);
    while(thing_it != visible_thing_scratch.end()) {
        draw_thing(currentModel->get_thing(std::get<0>(*thing_it)), std::get<0>(*thing_it));
        ++thing_it;
    }

    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);
    while(surf_it != translucent_surfaces_scratch.end()) {
        draw_surface(std::get<1>(*surf_it), currentModel->sectors[std::get<0>(*surf_it)],
                (currentModel->surfaces[std::get<1>(*surf_it)].face_type_flags & flags::face_flag::Translucent) ? 0.5f : 1.0f);
        ++surf_it;
    }
}

void gorc::client::world::level_view::draw(const gorc::time&, const box<2, int>& view_size, graphics::render_target&) {
    if(currentModel) {
        const auto& cam = currentModel->camera_model.current_computed_state;

        glDisable(GL_STENCIL_TEST);
        glDisable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Clear surface scratch space
        horizon_sky_surfaces_scratch.clear();
        ceiling_sky_surfaces_scratch.clear();
        translucent_surfaces_scratch.clear();
        visible_thing_scratch.clear();

        // Set up world and projection matrices
        double aspect = static_cast<double>(get_size<0>(view_size)) / static_cast<double>(get_size<1>(view_size));

        projection_matrix = make_perspective_matrix(70.0f, static_cast<float>(aspect), 0.001f, 1000.0f);
        view_matrix = make_look_matrix(cam.position, cam.look, cam.up);

        while(!model_matrix_stack.empty()) {
            model_matrix_stack.pop();
        }
        model_matrix_stack.push(make_identity_matrix<4, float>());

        // Run level hidden surface removal
        compute_visible_sectors(view_size);
        record_visible_special_surfaces();
        record_visible_things();

        // Prepare for rendering ordinary surfaces
        auto sector_tint = currentModel->sectors[cam.containing_sector].tint;
        sector_tint = (sector_tint * length(sector_tint)) + (currentModel->dynamic_tint * (1.0f - length(sector_tint)));

        set_current_shader(surfaceShader, sector_tint);

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);

        // Render ordinary surfaces and enqueue translucent/sky.
        draw_visible_diffuse_surfaces();

        // Render skies
        draw_visible_sky_surfaces(view_size, sector_tint);

        // Interleave rendering things and translucent surfaces.
        glEnable(GL_BLEND);
        set_current_shader(surfaceShader, sector_tint);

        draw_visible_translucent_surfaces_and_things();

        // Draw lights
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        for(const auto& light_thing : visible_thing_scratch) {
            const auto& thing = currentModel->get_thing(std::get<0>(light_thing));

            float light = thing.light + ((thing.actor_flags & flags::actor_flag::HasFieldlight) ? thing.light_intensity : 0.0f);

            if(light <= 0.0f) {
                continue;
            }

            set_current_shader(lightShader, thing.position + thing.orient.transform(thing.light_offset),
                    cam.position, light, light);

            draw_visible_diffuse_surfaces();
            draw_visible_translucent_surfaces_and_things();
        }

        // Draw POV model:
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_DEPTH_BUFFER_BIT);
        set_current_shader(surfaceShader, sector_tint);
        draw_pov_model();

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        for(const auto& light_thing : visible_thing_scratch) {
            const auto& thing = currentModel->get_thing(std::get<0>(light_thing));

            float light = thing.light + ((thing.actor_flags & flags::actor_flag::HasFieldlight) ? thing.light_intensity : 0.0f);

            if(light <= 0.0f) {
                continue;
            }

            set_current_shader(lightShader, thing.position + thing.orient.transform(thing.light_offset),
                    cam.position, light, light);

            draw_pov_model();
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_DEPTH_TEST);
    }
}

void gorc::client::world::level_view::draw_pov_model() {
    // Draw POV model.
    const auto& cam = currentModel->camera_model.current_computed_state;

    if(cam.draw_pov_model) {
        maybe_if(currentModel->camera_model.pov_model, [&](auto pov_model) {
            const auto& thing = currentModel->get_thing(currentPresenter->get_local_player_thing());
            const auto& current_sector = currentModel->sectors[thing.sector];
            auto sector_color = make_fill_vector<3, float>(1.0f);
            maybe_if(current_sector.cmp, [&](auto cmp) {
                sector_color = cmp->tint_color;
            });

            float sector_light = current_sector.ambient_light + current_sector.extra_light;
            auto lit_sector_color = extend_vector<4>(sector_color * sector_light, 1.0f);

            int saber_mesh_node = -1;
            if(thing.jk_flags & flags::jk_flag::has_saber) {
                saber_mesh_node = 5;
            }

            pov_mesh_node_visitor v(lit_sector_color, *this, saber_mesh_node,
                    thing.saber_drawn_length, thing.saber_base_rad, thing.saber_tip_rad,
                    thing.saber_side_mat, thing.saber_tip_mat);
            auto pov_orient = thing.orient * make_rotation(make_vector(1.0f, 0.0f, 0.0f), thing.head_pitch);
            auto pov_model_offset = pov_orient * make_euler(cam.pov_model_offset);
            currentPresenter->key_presenter->visit_mesh_hierarchy(v, pov_model, thing.position,
                    pov_model_offset, currentModel->camera_model.pov_key_mix_id);
        });
    }
}

void gorc::client::world::level_view::draw_surface(unsigned int surf_num, const content::assets::level_sector& sector, float alpha) {
    const auto& surface = currentModel->surfaces[surf_num];
    const auto& lev = *currentModel->level;

    if(surface.material >= 0) {
        const auto& material_entry = currentModel->level->materials[surface.material];
        const auto& material = std::get<0>(material_entry).get_value();

        vector<2> tex_scale = make_vector(1.0f / static_cast<float>(get_size<0>(material->size)),
                1.0f / static_cast<float>(get_size<1>(material->size)));

        int surfaceCelNumber = surface.cel_number;
        int actualSurfaceCelNumber;
        if(surfaceCelNumber >= 0) {
            actualSurfaceCelNumber = surfaceCelNumber % static_cast<int>(material->cels.size());
        }
        else {
            // TODO: Add MaterialAnim.
            actualSurfaceCelNumber = 0;
            //actualSurfaceCelNumber = currentModel->MaterialCelNumber[surface.material] % material->Cels.size();
        }

        glActiveTexture(GL_TEXTURE0);
        graphics::bind_texture(material->cels[actualSurfaceCelNumber].diffuse);
        glActiveTexture(GL_TEXTURE1);
        graphics::bind_texture(material->cels[actualSurfaceCelNumber].light);

        glBegin(GL_TRIANGLES);

        auto sector_tint = make_zero_vector<3, float>();
        maybe_if(sector.cmp, [&](auto cmp) {
            sector_tint = cmp->tint_color;
        });

        vector<3> first_geo = lev.vertices[std::get<0>(surface.vertices[0])];
        vector<2> first_tex = lev.texture_vertices[std::get<1>(surface.vertices[0])] + surface.texture_offset;
        float first_intensity = std::get<2>(surface.vertices[0]) + sector.extra_light + surface.extra_light;
        auto first_color = extend_vector<4>(sector_tint * first_intensity, alpha);

        for(size_t i = 2; i < surface.vertices.size(); ++i) {
            vector<3> second_geo = lev.vertices[std::get<0>(surface.vertices[i - 1])];
            vector<2> second_tex = lev.texture_vertices[std::get<1>(surface.vertices[i - 1])] + surface.texture_offset;
            float second_intensity = std::get<2>(surface.vertices[i - 1]) + sector.extra_light + surface.extra_light;
            auto second_color = extend_vector<4>(sector_tint * second_intensity, alpha);

            vector<3> third_geo = lev.vertices[std::get<0>(surface.vertices[i])];
            vector<2> third_tex = lev.texture_vertices[std::get<1>(surface.vertices[i])] + surface.texture_offset;
            float third_intensity = std::get<2>(surface.vertices[i]) + sector.extra_light + surface.extra_light;
            auto third_color = extend_vector<4>(sector_tint * third_intensity, alpha);

            apply(glNormal3f, surface.normal);
            glTexCoord2f(get<0>(first_tex) * get<0>(tex_scale), get<1>(first_tex) * get<1>(tex_scale));
            apply(glColor4f, first_color);
            apply(glVertex3f, first_geo);

            apply(glNormal3f, surface.normal);
            glTexCoord2f(get<0>(second_tex) * get<0>(tex_scale), get<1>(second_tex) * get<1>(tex_scale));
            apply(glColor4f, second_color);
            apply(glVertex3f, second_geo);

            apply(glNormal3f, surface.normal);
            glTexCoord2f(get<0>(third_tex) * get<0>(tex_scale), get<1>(third_tex) * get<1>(tex_scale));
            apply(glColor4f, third_color);
            apply(glVertex3f, third_geo);
        }

        glEnd();
    }
}

void gorc::client::world::level_view::draw_saber(asset_ref<content::assets::material> saber_tip, asset_ref<content::assets::material> saber_blade,
        float saber_length, float saber_base_radius, float saber_tip_radius) {
    float tex_y = static_cast<float>(rand);

    draw_sprite(make_vector(0.0f, 0.0f, 0.0f), saber_tip, 0, saber_base_radius * 2.0f, saber_base_radius * 2.0f,
            flags::geometry_mode::solid, flags::light_mode::fully_lit, 1.0f, make_zero_vector<3, float>(), 0.0f);
    draw_sprite(make_vector(0.0f, 1.0f, 0.0f) * saber_length, saber_tip, 0, saber_tip_radius * 2.0f, saber_tip_radius * 2.0f,
            flags::geometry_mode::solid, flags::light_mode::fully_lit, 1.0f, make_zero_vector<3, float>(), 0.0f);

    // Draw saber blade:
    push_matrix();

    float light = 1.0f;

    auto vmat = model_matrix_stack.top();

    auto hilt_pos = vmat.transform(make_vector(0.0f, 0.0f, 0.0f));
    auto blade_dir = normalize(vmat.transform_normal(make_vector(0.0f, 1.0f, 0.0f)));
    auto blade_face = normalize(vmat.transform_normal(make_vector(0.0f, 0.0f, 1.0f)));
    auto blade_opp = cross(blade_dir, blade_face);
    auto cam_pos = currentModel->camera_model.current_computed_state.position;

    // Project cam_pos into saber hilt plane
    cam_pos = cam_pos - hilt_pos;
    cam_pos = cam_pos - blade_dir * dot(cam_pos, blade_dir);
    cam_pos = normalize(cam_pos);

    // Change basis
    float cam_pos_x = dot(blade_opp, cam_pos);
    float cam_pos_y = dot(blade_face, cam_pos);

    float angle = to_degrees(atan2f(cam_pos_x, cam_pos_y));
    concatenate_matrix(make_rotation_matrix(angle, make_vector(0.0f, 1.0f, 0.0f)));

    glActiveTexture(GL_TEXTURE0);
    graphics::bind_texture(saber_blade->cels[0].diffuse);
    glActiveTexture(GL_TEXTURE1);
    graphics::bind_texture(saber_blade->cels[0].light);

    vector<3> sprite_middle = make_zero_vector<3, float>();//sprite.Offset;
    vector<3> horiz_off = make_vector(1.0f, 0.0f, 0.0f);
    vector<3> vert_off = make_vector(0.0f, 1.0f, 0.0f) * saber_length;

    vector<3> sprite_vx1 = sprite_middle + horiz_off * saber_tip_radius + vert_off;
    vector<3> sprite_vx2 = sprite_middle + horiz_off * saber_base_radius;
    vector<3> sprite_vx3 = sprite_middle - horiz_off * saber_base_radius;
    vector<3> sprite_vx4 = sprite_middle - horiz_off * saber_tip_radius + vert_off;

    vector<3> sprite_normal = make_vector(0.0f, 1.0f, 0.0f);

    auto light_vec = make_vector(light, light, light, 1.0f);

    glDepthMask(GL_FALSE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glBegin(GL_QUADS);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(1, tex_y);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx4);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(1, 1.0f + tex_y);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx3);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(0, 1.0f + tex_y);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx2);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(0, tex_y);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx1);

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_POLYGON_OFFSET_FILL);

    pop_matrix();
}

void gorc::client::world::level_view::draw_sprite(const vector<3>& pos, asset_ref<content::assets::material> mat, int frame, float width, float height,
        flags::geometry_mode, flags::light_mode light_mode, float extra_light, const vector<3>& spr_offset, float sector_light) {
    // TODO: Deal with geo
    push_matrix();

    float light = sector_light + extra_light;
    if(light_mode == flags::light_mode::fully_lit) {
        light = 1.0f;
    }

    concatenate_matrix(make_translation_matrix(pos + spr_offset));

    matrix<4> new_model = (view_matrix * model_matrix_stack.top()).transpose();
    get<3, 0>(new_model) = 0.0f;
    get<3, 1>(new_model) = 0.0f;
    get<3, 2>(new_model) = 0.0f;
    get<3, 3>(new_model) = 1.0f;
    concatenate_matrix(new_model);

    update_shader_model_matrix();

    int current_frame = frame;

    glActiveTexture(GL_TEXTURE0);
    graphics::bind_texture(mat->cels[current_frame].diffuse);
    glActiveTexture(GL_TEXTURE1);
    graphics::bind_texture(mat->cels[current_frame].light);

    vector<3> sprite_middle = make_zero_vector<3, float>();//sprite.Offset;
    vector<3> horiz_off = make_vector(1.0f,0.0f,0.0f) * width * 0.5f;
    vector<3> vert_off = make_vector(0.0f,1.0f,0.0f) * height * 0.5f;

    vector<3> sprite_vx1 = sprite_middle + horiz_off + vert_off;
    vector<3> sprite_vx2 = sprite_middle + horiz_off - vert_off;
    vector<3> sprite_vx3 = sprite_middle - horiz_off - vert_off;
    vector<3> sprite_vx4 = sprite_middle - horiz_off + vert_off;

    vector<3> sprite_normal = make_vector(0.0f, 1.0f, 0.0f);

    auto light_vec = make_vector(light, light, light, 1.0f);

    glDepthMask(GL_FALSE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    glBegin(GL_QUADS);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(1,0);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx1);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(1,1);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx2);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(0,1);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx3);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(0,0);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx4);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(0,0);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx4);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(0,1);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx3);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(1,1);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx2);

    apply(glNormal3f, sprite_normal);
    glTexCoord2f(1,0);
    apply(glColor4f, light_vec);
    apply(glVertex3f, sprite_vx1);

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_POLYGON_OFFSET_FILL);

    pop_matrix();
    update_shader_model_matrix();
}

void gorc::client::world::level_view::draw_sprite(const game::world::components::thing& thing, asset_ref<content::assets::sprite> sprite, float sector_light) {
    // TODO: Currently plays animation over duration of timer. Behavior should be verified.
    int current_frame = 0;
    if(thing.timer > 0.0f) {
        current_frame = static_cast<int>(static_cast<int>(std::floor(static_cast<float>(sprite->mat->cels.size()) * thing.time_alive / thing.timer)) % sprite->mat->cels.size());
    }

    const auto& cam = currentModel->camera_model.current_computed_state;
    vector<3> offset = cross(cam.look, cam.up) * get<0>(sprite->offset) +
            cam.look * get<1>(sprite->offset) +
            cam.up * get<2>(sprite->offset);

    draw_sprite(thing.position, sprite->mat, current_frame, sprite->width, sprite->height, sprite->geometry_mode, sprite->light_mode,
            sprite->extra_light, offset, sector_light);
}

void gorc::client::world::level_view::draw_thing(const game::world::components::thing& thing, int thing_id) {
    if((thing.flags & flags::thing_flag::Invisible) || thing_id == currentModel->camera_model.current_computed_state.focus_not_drawn_thing) {
        return;
    }

    const auto& current_sector = currentModel->sectors[thing.sector];
    auto sector_color = make_fill_vector<3, float>(1.0f);
    maybe_if(current_sector.cmp, [&](auto cmp) {
        sector_color = cmp->tint_color;
    });

    float sector_light = current_sector.ambient_light + current_sector.extra_light;
    auto lit_sector_color = extend_vector<4>(sector_color * sector_light, 1.0f);

    maybe_if(thing.model_3d, [&](auto model) {
        int weapon_mesh_node = -1;
        int saber_mesh_node_a = -1;
        int saber_mesh_node_b = -1;
        maybe_if(thing.pup, [&](auto pup) {
            weapon_mesh_node = pup->get_joint(flags::puppet_joint_type::primary_weapon_fire);
            if(thing.jk_flags & flags::jk_flag::has_saber) {
                saber_mesh_node_a = pup->get_joint(flags::puppet_joint_type::primary_weapon_fire);
                if(thing.jk_flags & flags::jk_flag::has_two_sabers) {
                    saber_mesh_node_b = pup->get_joint(flags::puppet_joint_type::secondary_weapon_fire);
                }
            }
        });

        thing_mesh_node_visitor v(lit_sector_color, *this, weapon_mesh_node, saber_mesh_node_a, saber_mesh_node_b,
                thing.weapon_mesh, thing.saber_drawn_length, thing.saber_base_rad, thing.saber_tip_rad,
                thing.saber_side_mat, thing.saber_tip_mat);
        currentPresenter->key_presenter->visit_mesh_hierarchy(v, model, thing.position, thing.orient, thing.attached_key_mix,
                thing.pup, thing.head_pitch);
    });

    maybe_if(thing.spr, [&](auto spr) {
        this->draw_sprite(thing, spr, sector_light);
    });
}
