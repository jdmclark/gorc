#include "animation_loader.h"
#include "content/assets/animation.h"
#include "framework/content/manager.h"
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>

const std::vector<boost::filesystem::path> gorc::content::loaders::animation_loader::asset_root_path = { "3do/key" };

namespace gorc {
namespace content {
namespace loaders {

void SkipToNextAnimationSection(text::tokenizer& tok) {
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

void ParseAnimationHeaderSection(assets::animation& anim, text::tokenizer& tok, manager& manager, diagnostics::report& report) {
	tok.assert_identifier("FLAGS");
	anim.flags = flag_set<flags::key_flag>(tok.get_number<uint32_t>());

	tok.assert_identifier("TYPE"); // Unknown
	tok.get_number<uint32_t>();

	tok.assert_identifier("FRAMES");
	anim.frame_count = tok.get_number<unsigned int>();

	tok.assert_identifier("FPS");
	anim.framerate = tok.get_number<double>();

	tok.assert_identifier("JOINTS");
	tok.get_number<unsigned int>();
}

void ParseMarkersSection(assets::animation& anim, text::tokenizer& tok, manager& manager, diagnostics::report& report) {
	tok.assert_identifier("MARKERS");
	unsigned int num_markers = tok.get_number<unsigned int>();

	for(unsigned int i = 0; i < num_markers; ++i) {
		double frame = tok.get_number<double>();
		anim.markers.emplace_back(frame, static_cast<flags::key_marker_type>(tok.get_number<uint32_t>()));
	}
}

void ParseKeyframeNodesSection(assets::animation& anim, text::tokenizer& tok, manager& manager, diagnostics::report& report) {
	tok.assert_identifier("NODES");

	tok.assert_identifier("NODES");
	unsigned int num_nodes = tok.get_number<unsigned int>();

	for(unsigned int i = 0; i < num_nodes; ++i) {
		anim.nodes.push_back(assets::animation_node());
		assets::animation_node& node = anim.nodes.back();

		tok.assert_identifier("NODE");
		tok.get_number<unsigned int>();

		tok.assert_identifier("MESH");
		tok.assert_identifier("NAME");
		tok.get_space_delimited_string();

		tok.assert_identifier("ENTRIES");
		unsigned int num_entries = tok.get_number<unsigned int>();

		for(unsigned int j = 0; j < num_entries; ++j) {
			node.frames.push_back(assets::animation_frame());
			assets::animation_frame& frame = node.frames.back();

			tok.get_number<int>();
			tok.assert_punctuator(":");

			frame.frame = tok.get_number<int>();

			tok.get_number<unsigned int>();

			float x = tok.get_number<float>();
			float y = tok.get_number<float>();
			float z = tok.get_number<float>();

			float pi = tok.get_number<float>();
			float ya = tok.get_number<float>();
			float ro = tok.get_number<float>();

			float dx = tok.get_number<float>();
			float dy = tok.get_number<float>();
			float dz = tok.get_number<float>();

			float dpi = tok.get_number<float>();
			float dya = tok.get_number<float>();
			float dro = tok.get_number<float>();

			frame.position = make_vector(x, y, z);
			frame.orientation = make_vector(pi, ya, ro);

			frame.delta_position = make_vector(dx, dy, dz);
			frame.delta_orientation = make_vector(dpi, dya, dro);
		}
	}
}

using AnimationLoaderSectionFn = std::function<void(assets::animation&, text::tokenizer&, manager&, diagnostics::report&)>;
const std::unordered_map<std::string, AnimationLoaderSectionFn> AnimationLoaderSectionMap {
	{ "header", ParseAnimationHeaderSection },
	{ "markers", ParseMarkersSection },
	{ "keyframe", ParseKeyframeNodesSection }
};

}
}
}

std::unique_ptr<gorc::content::asset> gorc::content::loaders::animation_loader::parse(text::tokenizer& tok, manager& manager, diagnostics::report& report) {
	std::unique_ptr<assets::animation> lev(new assets::animation());

	text::token t;
	while(true) {
		SkipToNextAnimationSection(tok);
		tok.get_token(t);

		if(t.type == text::token_type::end_of_file) {
			break;
		}
		else {
			std::transform(t.value.begin(), t.value.end(), t.value.begin(), tolower);
			auto it = AnimationLoaderSectionMap.find(t.value);
			if(it == AnimationLoaderSectionMap.end()) {
				report.add_warning("AnimationLoader", boost::str(boost::format("skipping unknown section %s") % t.value), t.location);
			}
			else {
				it->second(*lev, tok, manager, report);
			}
		}
	}

	return std::unique_ptr<asset>(std::move(lev));
}
