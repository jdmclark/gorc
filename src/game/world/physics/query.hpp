#pragma once

#include "libold/content/assets/level.hpp"
#include "game/world/surface.hpp"
#include "libold/content/assets/model.hpp"
#include "shape.hpp"
#include "contact.hpp"
#include "math/util.hpp"
#include "math/matrix.hpp"
#include "game/world/level_model.hpp"
#include "utility/maybe.hpp"

namespace gorc {
namespace game {
namespace world {
namespace physics {

template <typename VertexProvider, typename EdgeProvider> bool point_inside_surface(const vector<3>& sp,
        const VertexProvider& level, const EdgeProvider& surface, const matrix<4>& trns) {
    for(size_t i = surface.vertices.size() - 1UL, j = 0UL; j < surface.vertices.size(); i = j++) {
        auto p0 = trns.transform(level.vertices[std::get<0>(surface.vertices[i])]);
        auto edge = trns.transform(level.vertices[std::get<0>(surface.vertices[j])]) - p0;
        auto edge_normal = cross(trns.transform_normal(surface.normal), edge);
        if(dot(edge_normal, sp - p0) < 0.0f) {
            return false;
        }
    }

    return true;
}

template <typename VertexProvider, typename EdgeProvider> bool point_inside_surface(const vector<3>& sp,
        const VertexProvider& level, const EdgeProvider& surface) {
    for(size_t i = surface.vertices.size() - 1UL, j = 0UL; j < surface.vertices.size(); i = j++) {
        auto p0 = level.vertices[std::get<0>(surface.vertices[i])];
        auto edge = level.vertices[std::get<0>(surface.vertices[j])] - p0;
        auto edge_normal = cross(surface.normal, edge);
        if(dot(edge_normal, sp - p0) < 0.0f) {
            return false;
        }
    }

    return true;
}

template <typename VertexProvider, typename EdgeProvider> maybe<vector<3>> segment_surface_intersection_point(const segment& segment,
        const VertexProvider& level, const EdgeProvider& surface, const matrix<4>& trns) {
    // Check for point
    auto nrm = trns.transform_normal(surface.normal);
    auto p = trns.transform(level.vertices[std::get<0>(surface.vertices[0])]);
    auto u = dot(nrm, p - std::get<0>(segment)) / dot(nrm, std::get<1>(segment) - std::get<0>(segment));
    if(u < 0.0f || u > 1.0f) {
        return nothing;
    }

    // Check for thing path passing through surface polygon.
    auto sp = lerp(std::get<0>(segment), std::get<1>(segment), u);
    if(point_inside_surface(sp, level, surface, trns)) {
        return sp;
    }

    return nothing;
}

template <typename VertexProvider, typename EdgeProvider> maybe<vector<3>> segment_surface_intersection_point(const segment& segment,
        const VertexProvider& level, const EdgeProvider& surface) {
    // Check for point
    auto nrm = surface.normal;
    auto p = level.vertices[std::get<0>(surface.vertices[0])];
    auto u = dot(nrm, p - std::get<0>(segment)) / dot(nrm, std::get<1>(segment) - std::get<0>(segment));
    if(u < 0.0f || u > 1.0f) {
        return nothing;
    }

    // Check for thing path passing through surface polygon.
    auto sp = lerp(std::get<0>(segment), std::get<1>(segment), u);
    if(point_inside_surface(sp, level, surface)) {
        return sp;
    }

    return nothing;
}

template <typename VertexProvider, typename EdgeProvider> bool segment_surface_intersection(const segment& segment,
        const VertexProvider& level, const EdgeProvider& surface, const matrix<4>& trns) {
    // Check for point
    auto nrm = trns.transform_normal(surface.normal);
    auto p = trns.transform(level.vertices[std::get<0>(surface.vertices[0])]);
    auto u = dot(nrm, p - std::get<0>(segment)) / dot(nrm, std::get<1>(segment) - std::get<0>(segment));
    if(u < 0.0f || u > 1.0f) {
        return false;
    }

    // Check for thing path passing through surface polygon.
    auto sp = lerp(std::get<0>(segment), std::get<1>(segment), u);
    return point_inside_surface(sp, level, surface, trns);
}

template <typename VertexProvider, typename EdgeProvider> bool segment_surface_intersection(const segment& segment,
        const VertexProvider& level, const EdgeProvider& surface) {
    // Check for point
    auto nrm = surface.normal;
    auto p = level.vertices[std::get<0>(surface.vertices[0])];
    auto u = dot(nrm, p - std::get<0>(segment)) / dot(nrm, std::get<1>(segment) - std::get<0>(segment));
    if(u < 0.0f || u > 1.0f) {
        return false;
    }

    // Check for thing path passing through surface polygon.
    auto sp = lerp(std::get<0>(segment), std::get<1>(segment), u);
    return point_inside_surface(sp, level, surface);
}

template <typename VertexProvider, typename EdgeProvider> maybe<vector<3>> bounded_closest_point_on_surface(const vector<3>& origin,
        const VertexProvider& level, const EdgeProvider& surface, const matrix<4>& trns, float max_dist) {
    auto nrm = trns.transform_normal(surface.normal);
    auto p = trns.transform(level.vertices[std::get<0>(surface.vertices[0])]);
    auto v = origin - p;
    auto plane_dist = dot(nrm, v);
    if(plane_dist < 0.0f || plane_dist > max_dist) {
        return nothing;
    }

    auto pp = (v - nrm * plane_dist) + p;
    if(point_inside_surface(pp, level, surface, trns)) {
        return pp;
    }

    // Check edges
    float closest_dist = std::numeric_limits<float>::max();
    vector<3> closest_point = make_zero_vector<3, float>();

    for(size_t p0 = surface.vertices.size() - 1UL, p1 = 0UL; p1 < surface.vertices.size(); p0 = p1++) {
        auto vp0 = trns.transform(level.vertices[std::get<0>(surface.vertices[p0])]);
        auto vp1 = trns.transform(level.vertices[std::get<0>(surface.vertices[p1])]);
        auto lv = vp1 - vp0;
        auto pv = origin - vp0;

        vector<3> candidate_point;
        auto alpha = dot(lv, pv) / length_squared(lv);
        if(alpha < 0.0f) {
            candidate_point = vp0;
        }
        else if(alpha > 1.0f) {
            candidate_point = vp1;
        }
        else {
            candidate_point = lerp(vp0, vp1, alpha);
        }

        auto cp_dist = length(candidate_point - origin);
        if(cp_dist < closest_dist) {
            closest_point = candidate_point;
            closest_dist = cp_dist;
        }
    }

    return closest_point;
}

template <typename VertexProvider, typename EdgeProvider> maybe<vector<3>> bounded_closest_point_on_surface(const vector<3>& origin,
        const VertexProvider& level, const EdgeProvider& surface, float max_dist) {
    auto nrm = surface.normal;
    auto p = level.vertices[std::get<0>(surface.vertices[0])];
    auto v = origin - p;
    auto plane_dist = dot(nrm, v);
    if(plane_dist < 0.0f || plane_dist > max_dist) {
        return nothing;
    }

    auto pp = (v - nrm * plane_dist) + p;
    if(point_inside_surface(pp, level, surface)) {
        return pp;
    }

    // Check edges
    float closest_dist = std::numeric_limits<float>::max();
    vector<3> closest_point = make_zero_vector<3, float>();

    for(size_t p0 = surface.vertices.size() - 1UL, p1 = 0UL; p1 < surface.vertices.size(); p0 = p1++) {
        auto vp0 = level.vertices[std::get<0>(surface.vertices[p0])];
        auto vp1 = level.vertices[std::get<0>(surface.vertices[p1])];
        auto lv = vp1 - vp0;
        auto pv = origin - vp0;

        vector<3> candidate_point;
        auto alpha = dot(lv, pv) / length_squared(lv);
        if(alpha < 0.0f) {
            candidate_point = vp0;
        }
        else if(alpha > 1.0f) {
            candidate_point = vp1;
        }
        else {
            candidate_point = lerp(vp0, vp1, alpha);
        }

        auto cp_dist = length(candidate_point - origin);
        if(cp_dist < closest_dist) {
            closest_point = candidate_point;
            closest_dist = cp_dist;
        }
    }

    return closest_point;
}

bool point_inside_sector(const vector<3>& position, const level_model& level_model, const content::assets::level_sector& sec);

void segment_adjoin_path(const segment& segment, const level_model& level_model, const content::assets::level_sector& initial_sector,
        std::vector<std::tuple<sector_id, surface_id>>& path);

template <typename VertexProvider, typename EdgeProvider> maybe<float> swept_sphere_plane_collision_time(const swept_sphere& sphere,
        const VertexProvider& level, const EdgeProvider& surface, const matrix<4>& trns) {
    auto nrm = trns.transform_normal(surface.normal);
    auto p = trns.transform(level.vertices[std::get<0>(surface.vertices[0])]);
    auto u = dot(std::get<1>(sphere.position) - std::get<0>(sphere.position), nrm);
    if(u >= 0.0f) {
        return nothing;
    }

    return (sphere.radius - dot(std::get<0>(sphere.position) - p, nrm)) / u;
}

template <typename VertexProvider, typename EdgeProvider> maybe<float> swept_sphere_plane_collision_time(const swept_sphere& sphere,
        const VertexProvider& level, const EdgeProvider& surface) {
    auto nrm = surface.normal;
    auto p = level.vertices[std::get<0>(surface.vertices[0])];
    auto u = dot(std::get<1>(sphere.position) - std::get<0>(sphere.position), nrm);
    if(u >= 0.0f) {
        return nothing;
    }

    return (sphere.radius - dot(std::get<0>(sphere.position) - p, nrm)) / u;
}

inline maybe<vector<3>> segment_sphere_intersection(const segment& segment, const sphere& sphere) {
    auto v = std::get<1>(segment) - std::get<0>(segment);
    auto w = std::get<0>(segment) - sphere.position;

    auto a = dot(v, v);
    auto b = 2.0f * dot(v, w);
    auto c = dot(w, w) - (sphere.radius * sphere.radius);

    // (-b pm sqrt(b^2-4ac)) / 2a
    auto det = b * b - 4.0f * a * c;
    if(det <= 0.0f) {
        return nothing;
    }

    auto sqrt_det = sqrtf(det);
    auto sol_a = (-b + sqrt_det) / (2.0f * a);
    auto sol_b = (-b - sqrt_det) / (2.0f * a);

    bool has_sol = false;
    float selected_sol = 0.0f;

    if(sol_a >= 0.0f && sol_a <= 1.0f) {
        has_sol = true;
        selected_sol = sol_a;
    }

    if((!has_sol || sol_b < sol_a) && sol_b >= 0.0f && sol_b <= 1.0f) {
        has_sol = true;
        selected_sol = sol_b;
    }

    if(has_sol) {
        return lerp(std::get<0>(segment), std::get<1>(segment), selected_sol);
    }
    else {
        return nothing;
    }
}

}
}
}
}
