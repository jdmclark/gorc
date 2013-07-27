#include "animation_loader.h"
#include "content/assets/animation.h"
#include "content/manager.h"
#include <boost/format.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <unordered_map>
#include <string>
#include <tuple>
#include <vector>

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::AnimationLoader::AssetRootPath = { "3do/key" };

namespace Gorc {
namespace Content {
namespace Loaders {

void SkipToNextAnimationSection(Text::Tokenizer& tok) {
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

void ParseAnimationHeaderSection(Assets::Animation& anim, Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	tok.AssertIdentifier("FLAGS");
	anim.Flags = FlagSet<Flags::KeyFlag>(tok.GetNumber<uint32_t>());

	tok.AssertIdentifier("TYPE"); // Unknown
	tok.GetNumber<uint32_t>();

	tok.AssertIdentifier("FRAMES");
	anim.FrameCount = tok.GetNumber<unsigned int>();

	tok.AssertIdentifier("FPS");
	anim.FrameRate = tok.GetNumber<double>();

	tok.AssertIdentifier("JOINTS");
	tok.GetNumber<unsigned int>();
}

void ParseKeyframeNodesSection(Assets::Animation& anim, Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	tok.AssertIdentifier("NODES");

	tok.AssertIdentifier("NODES");
	unsigned int num_nodes = tok.GetNumber<unsigned int>();

	for(unsigned int i = 0; i < num_nodes; ++i) {
		anim.Nodes.push_back(Assets::AnimationNode());
		Assets::AnimationNode& node = anim.Nodes.back();

		tok.AssertIdentifier("NODE");
		tok.GetNumber<unsigned int>();

		tok.AssertIdentifier("MESH");
		tok.AssertIdentifier("NAME");
		tok.GetSpaceDelimitedString();

		tok.AssertIdentifier("ENTRIES");
		unsigned int num_entries = tok.GetNumber<unsigned int>();

		for(unsigned int j = 0; j < num_entries; ++j) {
			node.Frames.push_back(Assets::AnimationFrame());
			Assets::AnimationFrame& frame = node.Frames.back();

			tok.GetNumber<int>();
			tok.AssertPunctuator(":");

			frame.Frame = tok.GetNumber<int>();

			tok.GetNumber<unsigned int>();

			float x = tok.GetNumber<float>();
			float y = tok.GetNumber<float>();
			float z = tok.GetNumber<float>();

			float pi = tok.GetNumber<float>();
			float ya = tok.GetNumber<float>();
			float ro = tok.GetNumber<float>();

			float dx = tok.GetNumber<float>();
			float dy = tok.GetNumber<float>();
			float dz = tok.GetNumber<float>();

			float dpi = tok.GetNumber<float>();
			float dya = tok.GetNumber<float>();
			float dro = tok.GetNumber<float>();

			frame.Position = Math::Vec(x, y, z);
			frame.Orientation = Math::Vec(pi, ya, ro);

			frame.DeltaPosition = Math::Vec(dx, dy, dz);
			frame.DeltaOrientation = Math::Vec(dpi, dya, dro);
		}
	}
}

using AnimationLoaderSectionFn = std::function<void(Assets::Animation&, Text::Tokenizer&, Manager&, Diagnostics::Report&)>;
const std::unordered_map<std::string, AnimationLoaderSectionFn> AnimationLoaderSectionMap {
	{ "header", ParseAnimationHeaderSection },
	{ "keyframe", ParseKeyframeNodesSection }
};

}
}
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::AnimationLoader::Parse(Text::Tokenizer& tok, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Assets::Animation> lev(new Assets::Animation());

	Text::Token t;
	while(true) {
		SkipToNextAnimationSection(tok);
		tok.GetToken(t);

		if(t.Type == Text::TokenType::EndOfFile) {
			break;
		}
		else {
			std::transform(t.Value.begin(), t.Value.end(), t.Value.begin(), tolower);
			auto it = AnimationLoaderSectionMap.find(t.Value);
			if(it == AnimationLoaderSectionMap.end()) {
				report.AddWarning("AnimationLoader", boost::str(boost::format("skipping unknown section %s") % t.Value), t.Location);
			}
			else {
				it->second(*lev, tok, manager, report);
			}
		}
	}

	return std::unique_ptr<Asset>(std::move(lev));
}
