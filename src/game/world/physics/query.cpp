#include "query.h"
#include "framework/math/util.h"

bool gorc::game::world::physics::point_inside_sector(const vector<3>& position, const level_model& model,
		const gorc::content::assets::level_sector& sec) {
	for(size_t i =  sec.first_surface; i < sec.first_surface + sec.surface_count; ++i) {
		const auto& surf = model.level.surfaces[i];
		const auto& p = model.level.vertices[std::get<0>(surf.vertices[0])];

		if(dot(surf.normal, position - p) < 0.0f) {
			return false;
		}
	}

	return true;
}

void gorc::game::world::physics::segment_adjoin_path(const segment& segment, const level_model& level,
		const content::assets::level_sector& initial_sector, std::vector<std::tuple<int, int>>& path) {
	path.clear();

	auto segment_dir = std::get<1>(segment) - std::get<0>(segment);

	unsigned int current_sector_id = initial_sector.number;
	while(true) {
		const auto& current_sector = level.sectors[current_sector_id];

		if(point_inside_sector(std::get<1>(segment), level, current_sector)) {
			path.emplace_back(current_sector_id, -1);
			return;
		}

		bool has_continued = false;
		for(int surf_id = current_sector.first_surface; surf_id < current_sector.first_surface + current_sector.surface_count; ++surf_id) {
			auto& surf = level.surfaces[surf_id];
			if(surf.adjoin >= 0 && dot(surf.normal, segment_dir) <= 0.0f && segment_surface_intersection(segment, level.level,
					surf, make_identity_matrix<4, float>())) {
				// Object passes through this adjoin, to the adjoined sector.
				path.emplace_back(current_sector_id, surf_id);
				current_sector_id = surf.adjoined_sector;
				has_continued = true;
				break;
			}
		}

		if(!has_continued) {
			// Object has exited current sector. Abort.
			// TODO: Print error, or recover with random walk.
			path.emplace_back(current_sector_id, -1);
			return;
		}
	}
}
