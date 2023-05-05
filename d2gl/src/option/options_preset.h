#pragma once
#include <string>;
#include <vector>

namespace d2gl {
class options_preset {
public:
	options_preset(std::string name, bool vsync, bool fxaa, bool sharpen, bool bloom, bool motion_prediction)
	{
		preset_name = name;
		enable_vsync = vsync;
		enable_fxaa = fxaa;
		enable_sharpen = sharpen;
		enable_bloom = bloom;
		enable_motion_prediction = motion_prediction;
	}
	std::string preset_name;
	bool enable_vsync;
	bool enable_fxaa;
	bool enable_sharpen;
	bool enable_bloom;
	bool enable_motion_prediction;
};
}
