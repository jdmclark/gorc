#include "template.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "content/manager.h"
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace Gorc {
namespace Content {
namespace Assets {

static const std::unordered_map<std::string, ThingType> TemplateTypeMap {
	{ "actor", ThingType::Actor },
	{ "weapon", ThingType::Weapon },
	{ "debris", ThingType::Debris },
	{ "item", ThingType::Item },
	{ "explosion", ThingType::Explosion },
	{ "cog", ThingType::Cog },
	{ "ghost", ThingType::Ghost },
	{ "corpse", ThingType::Corpse },
	{ "player", ThingType::Player },
	{ "particle", ThingType::Particle }
};

static const std::unordered_map<std::string, MoveType> MoveTypeMap {
	{ "none", MoveType::None },
	{ "physics", MoveType::Physics },
	{ "path", MoveType::Path }
};

using TemplateParameterParser = std::function<void(Template&, Text::Tokenizer&, Content::Manager&, const Colormap&, Diagnostics::Report&)>;

template <typename T> void TemplateParameterValueMapper(const std::unordered_map<std::string, T>& map, T& value,
		const T& defaultValue, Text::Tokenizer& tok, Diagnostics::Report& report) {
	std::string key = tok.GetSpaceDelimitedString();
	std::transform(key.begin(), key.end(), key.begin(), tolower);
	auto it = map.find(key);
	if(it == map.end()) {
		report.AddWarning("Template::ParseArgs::TemplateParameterValueMapper", boost::str(boost::format("unknown parameter value \'%s\'") % key),
				tok.GetInternalTokenLocation());
		value = defaultValue;
	}
	else {
		value = it->second;
	}
}

template <typename T> void TemplateParameterNumberMapper(T& value, const T& defaultValue, Text::Tokenizer& tok, Diagnostics::Report& report) {
	Text::Token t;
	tok.GetToken(t);

	if(t.Type == Text::TokenType::Float || t.Type == Text::TokenType::Integer || t.Type == Text::TokenType::HexInteger) {
		value = t.GetNumericValue<T>();
	}
	else {
		report.AddWarning("Template::ParseArgs::TemplateParameterNumberMapper", "expected numeric value", t.Location);
		value = defaultValue;
	}
}

template <typename T> void TemplateParameterEnumMapper(T& value, const T& defaultValue, Text::Tokenizer& tok, Diagnostics::Report& report) {
	Text::Token t;
	tok.GetToken(t);

	if(t.Type == Text::TokenType::Float || t.Type == Text::TokenType::Integer || t.Type == Text::TokenType::HexInteger) {
		value = static_cast<T>(t.GetNumericValue<typename std::underlying_type<T>::type>());
	}
	else {
		report.AddWarning("Template::ParseArgs::TemplateParameterEnumMapper", "expected numeric value", t.Location);
		value = defaultValue;
	}
}

template <typename T> void TemplateParameterFlagMapper(FlagSet<T>& value, const FlagSet<T>& defaultValue, Text::Tokenizer& tok, Diagnostics::Report& report) {
	Text::Token t;
	tok.GetToken(t);

	if(t.Type == Text::TokenType::Float || t.Type == Text::TokenType::Integer || t.Type == Text::TokenType::HexInteger) {
		value = FlagSet<T>(t.GetNumericValue<typename std::underlying_type<T>::type>());
	}
	else {
		report.AddWarning("Template::ParseArgs::TemplateParameterFlagMapper", "expected numeric value", t.Location);
		value = defaultValue;
	}
}

void TemplateParameterVectorMapper(Math::Vector<3>& value, Text::Tokenizer& tok) {
	tok.AssertPunctuator("(");
	Math::Get<0>(value) = tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	Math::Get<1>(value) = tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	Math::Get<2>(value) = tok.GetNumber<float>();
	tok.AssertPunctuator(")");
}

void TemplateModel3DParser(Template& tpl, Text::Tokenizer& tok, Content::Manager& manager, const Colormap& cmp, Diagnostics::Report& report) {
	std::string fn = tok.GetSpaceDelimitedString();
	if(boost::iequals(fn, "none")) {
		tpl.Model3d = nullptr;
	}
	else {
		tpl.Model3d = &manager.Load<Model>(fn, cmp);
	}
}

void TemplateSoundClassParser(Template& tpl, Text::Tokenizer& tok, Content::Manager& manager, const Colormap&, Diagnostics::Report& report) {
	std::string fn = tok.GetSpaceDelimitedString();
	if(boost::iequals(fn, "none")) {
		tpl.SoundClass = nullptr;
	}
	else {
		tpl.SoundClass = &manager.Load<SoundClass>(fn);
	}
}

void TemplateAddFrame(Template& tpl, Text::Tokenizer& tok) {
	tok.AssertPunctuator("(");
	float x = tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	float y = tok.GetNumber<float>();
	tok.AssertPunctuator("/");
	float z = tok.GetNumber<float>();

	Text::Token t;
	tok.GetToken(t);

	if(t.Value == ":") {
		float p = tok.GetNumber<float>();
		tok.AssertPunctuator("/");
		float a = tok.GetNumber<float>();
		tok.AssertPunctuator("/");
		float r = tok.GetNumber<float>();
		tok.AssertPunctuator(")");

		tpl.Frames.emplace_back(Math::Vec(x, y, z), Math::Vec(p, a, r));
	}
	else {
		tpl.Frames.emplace_back(Math::Vec(x, y, z), Math::Zero<3>());
	}
}

static const std::unordered_map<std::string, TemplateParameterParser> TemplateParameterParserMap {
	{ "model3d", &TemplateModel3DParser },
	{ "soundclass", &TemplateSoundClassParser },
	{ "type", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report& report) {
		TemplateParameterValueMapper(TemplateTypeMap, tpl.Type, ThingType::Free, tok, report); }},
	{ "move", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report& report) {
		TemplateParameterValueMapper(MoveTypeMap, tpl.Move, MoveType::None, tok, report); }},
	{ "mass", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report& report) {
		TemplateParameterNumberMapper(tpl.Mass, 2.0f, tok, report); }},
	{ "movesize", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report& report) {
		TemplateParameterNumberMapper(tpl.MoveSize, 0.05f, tok, report); }},
	{ "size", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report& report) {
			TemplateParameterNumberMapper(tpl.Size, 0.05f, tok, report); }},
	{ "eyeoffset", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report&) {
		TemplateParameterVectorMapper(tpl.EyeOffset, tok); }},
	{ "collide", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report& report) {
		TemplateParameterEnumMapper(tpl.Collide, CollideType::None, tok, report); }},
	{ "thingflags", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report& report) {
		TemplateParameterFlagMapper(tpl.Flags, FlagSet<ThingFlag>(), tok, report); }},
	{ "numframes", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report&) {
		/* Silently consume numframes */ tok.GetNumber<int>(); }},
	{ "frame", [](Template& tpl, Text::Tokenizer& tok, Content::Manager&, const Colormap&, Diagnostics::Report&) {
		TemplateAddFrame(tpl, tok); }}
};

}
}
}

void Gorc::Content::Assets::Template::ParseArgs(Text::Tokenizer& tok, Content::Manager& manager, const Colormap& cmp, Diagnostics::Report& report) {
	bool oldReportEOL = tok.GetReportEOL();
	tok.SetReportEOL(true);

	Text::Token t;
	while(true) {
		tok.GetToken(t);

		if(t.Type == Text::TokenType::EndOfFile || t.Type == Text::TokenType::EndOfLine) {
			break;
		}
		else {
			tok.AssertPunctuator("=");

			std::transform(t.Value.begin(), t.Value.end(), t.Value.begin(), tolower);
			auto it = TemplateParameterParserMap.find(t.Value);
			if(it == TemplateParameterParserMap.end()) {
				report.AddWarning("Template::ParseArgs", boost::str(boost::format("unrecognized template param \'%s\'") % t.Value), t.Location);
				tok.GetSpaceDelimitedString();
			}
			else {
				it->second(*this, tok, manager, cmp, report);
			}
		}
	}

	tok.SetReportEOL(oldReportEOL);
}
