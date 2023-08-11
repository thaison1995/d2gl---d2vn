#pragma once
#include "pch.h"
#include "config.h"
#include "yaml-cpp/yaml.h"

namespace d2gl {

int Config::GetInt(std::string key, int def, int min = INT_MIN, int max = INT_MAX)
{
	int val = def;
	if (yaml[key]) {
		val = yaml[key].as<int>();
	}

	if (min != INT_MIN && val < min) {
		val = min;
	}

	if (max != INT_MAX && val > max) {
		val = max;
	}

	return val;
}

float Config::GetFloat(std::string key, float def, float min, float max)
{
	float val = def;
	if (yaml[key]) {
		val = yaml[key].as<float>();
	}

	if (val < min) {
		val = min;
	}

	if (val > max) {
		val = max;
	}

	return val;
}

bool Config::GetBool(std::string key, bool def)
{
	bool val = def;
	if (yaml[key]) {
		val = yaml[key].as<bool>();
	}
	return val;
}

std::string Config::GetString(std::string key, const std::string& def)
{
	std::string val = def;
	if (yaml[key]) {
		val = yaml[key].as<std::string>();
	}
	return val;
}

void Config::SaveConfig()
{
	yaml["window_size_width"] = App.window.size.x;
	yaml["window_size_height"] = App.window.size.y;
	yaml["window_fullscreen"] = App.window.fullscreen;
	yaml["auto_minimize"] = App.window.auto_minimize;
	yaml["window_dark_mode"] = App.window.dark_mode;
	yaml["window_vsync"] = App.vsync;
	yaml["unlock_cursor"] = App.cursor.unlock;
	yaml["window_centered"] = App.window.centered;
	yaml["window_position_x"] = App.window.position.x;
	yaml["window_position_y"] = App.window.position.y;
	yaml["foreground_fps_active"] = App.foreground_fps.active;
	yaml["foreground_fps_value"] = App.foreground_fps.range.value;
	yaml["background_fps_active"] = App.background_fps.active;
	yaml["background_fps_value"] = App.background_fps.range.value;
	yaml["shader_preset"] = App.shader.preset;
	yaml["lut"] = App.lut.selected;
	yaml["fxaa"] = App.fxaa.active;
	yaml["fxaa_preset"] = App.fxaa.presets.selected;
	yaml["sharpen"] = App.sharpen.active;
	yaml["sharpen_strength"] = App.sharpen.strength.value;
	yaml["sharpen_clamp"] = App.sharpen.clamp.value;
	yaml["sharpen_radius"] = App.sharpen.radius.value;
	yaml["bloom"] = App.bloom.active;
	yaml["bloom_exposure"] = App.bloom.exposure.value;
	yaml["bloom_gamma"] = App.bloom.gamma.value;
	yaml["stretched_horizontal"] = App.viewport.stretched.x;
	yaml["stretched_vertical"] = App.viewport.stretched.y;
	yaml["hd_cursor"] = App.hd_cursor;
	yaml["hd_text"] = App.hd_text.active;
	yaml["hd_text_scale"] = App.hd_text.scale.value;
	yaml["hd_orbs"] = App.hd_orbs.active;
	yaml["hd_orbs_centered"] = App.hd_orbs.centered;
	yaml["mini_map"] = App.mini_map.active;
	yaml["mini_map_text_over"] = App.mini_map.text_over;
	yaml["mini_map_width"] = App.mini_map.width.value;
	yaml["mini_map_height"] = App.mini_map.height.value;

	yaml["motion_prediction"] = App.motion_prediction;
	yaml["skip_intro"] = App.skip_intro;
	yaml["no_pickup"] = App.no_pickup;
	yaml["show_item_quantity"] = App.show_item_quantity;
	yaml["show_fps"] = App.show_fps;
	yaml["gl_ver_major"] = (int)App.gl_ver.x;
	yaml["gl_ver_minor"] = (int)App.gl_ver.y;
	yaml["use_compute_shader"] = App.use_compute_shader;
	yaml["load_dlls_early"] = App.dlls_early;
	yaml["load_dlls_late"] = App.dlls_late;

	std::ofstream fout(App.yaml_file);
	fout << yaml;

	fout.close();
}

void Config::LoadConfig()
{
	bool bCreateFile = false;
	std::ifstream ifile(App.yaml_file);
	if (!ifile) {
		std::ofstream output(App.yaml_file);
		bCreateFile = true;
	}

	yaml = YAML::LoadFile(App.yaml_file);

	// Setup Color Grading profiles
	App.lut.items.push_back({ "Game Default" });
	for (int i = 1; i <= 14; i++) {
		char label[50] = { 0 };
		sprintf_s(label, 50, "Color #%d", i);
		App.lut.items.push_back({ label });
	}

	// Resolution
	App.desktop_resolution = {
		GetSystemMetrics(SM_XVIRTUALSCREEN),
		GetSystemMetrics(SM_YVIRTUALSCREEN),
		GetSystemMetrics(SM_CXVIRTUALSCREEN),
		GetSystemMetrics(SM_CYVIRTUALSCREEN)
	};
	App.window.size = App.desktop_resolution;
	App.window.size.x = d2gl::Config::GetInt("window_size_width", App.window.size.x, 800, App.desktop_resolution.z);
	App.window.size.y = d2gl::Config::GetInt("window_size_height", App.window.size.y, 600, App.desktop_resolution.w);
	App.window.size_save = App.window.size;

	App.window.fullscreen = d2gl::Config::GetBool("window_fullscreen", true);
	App.window.auto_minimize = d2gl::Config::GetBool("auto_minimize", App.window.auto_minimize);
	App.window.dark_mode = d2gl::Config::GetBool("window_dark_mode", true);
	App.vsync = d2gl::Config::GetBool("window_vsync", true);
	App.window.centered = d2gl::Config::GetBool("window_centered", true);
	App.cursor.unlock = d2gl::Config::GetBool("unlock_cursor", App.cursor.unlock);

	// Window Position
	App.window.position.x = d2gl::Config::GetInt("window_position_x", App.window.position.x, App.desktop_resolution.x, App.desktop_resolution.z);
	App.window.position.y = d2gl::Config::GetInt("window_position_y", App.window.position.y, App.desktop_resolution.y, App.desktop_resolution.w);

	App.foreground_fps.active = d2gl::Config::GetBool("foreground_fps_active", true);
	App.foreground_fps.range.value = d2gl::Config::GetInt("foreground_fps_value", App.foreground_fps.range.value, App.foreground_fps.range.min, App.foreground_fps.range.max);
	App.background_fps.active = d2gl::Config::GetBool("background_fps_active", true);
	App.background_fps.range.value = d2gl::Config::GetInt("background_fps_value", App.background_fps.range.value, App.background_fps.range.min, App.background_fps.range.max);

	App.shader.preset = d2gl::Config::GetString("shader_preset", App.shader.preset);

	App.lut.selected = d2gl::Config::GetInt("lut", App.lut.selected, 0, App.lut.items.size() - 1);
	App.fxaa.active = d2gl::Config::GetBool("fxaa", App.fxaa.active);
	App.fxaa.presets.selected = d2gl::Config::GetInt("fxaa_preset", App.fxaa.presets.selected, 0, 2);

	App.sharpen.active = d2gl::Config::GetBool("sharpen", App.sharpen.active);
	App.sharpen.strength.value = d2gl::Config::GetFloat("sharpen_strength", App.sharpen.strength.value, App.sharpen.strength.min, App.sharpen.strength.max);
	App.sharpen.clamp.value = d2gl::Config::GetFloat("sharpen_clamp", App.sharpen.clamp.value, App.sharpen.clamp.min, App.sharpen.clamp.max);
	App.sharpen.radius.value = d2gl::Config::GetFloat("sharpen_radius", App.sharpen.radius.value, App.sharpen.radius.min, App.sharpen.radius.max);

	App.bloom.active = d2gl::Config::GetBool("bloom", App.bloom.active);
	App.bloom.exposure.value = d2gl::Config::GetFloat("bloom_exposure", App.bloom.exposure.value, App.bloom.exposure.min, App.bloom.exposure.max);
	App.bloom.gamma.value = d2gl::Config::GetFloat("bloom_gamma", App.bloom.gamma.value, App.bloom.gamma.min, App.bloom.gamma.max);

	App.viewport.stretched.x = d2gl::Config::GetBool("stretched_horizontal", App.viewport.stretched.x);
	App.viewport.stretched.y = d2gl::Config::GetBool("stretched_vertical", App.viewport.stretched.y);

	App.hd_cursor = d2gl::Config::GetBool("hd_cursor", App.hd_cursor);
	App.hd_text.active = d2gl::Config::GetBool("hd_text", App.hd_text.active);
	App.hd_text.scale.value = ISHDTEXT() ? 1.0f : d2gl::Config::GetFloat("hd_text_scale", App.hd_text.scale.value, 0.8f, 1.2f);
	App.hd_orbs.active = d2gl::Config::GetBool("hd_orbs", App.hd_orbs.active);
	App.hd_orbs.centered = d2gl::Config::GetBool("hd_orbs_centered", App.hd_orbs.centered);
	App.mini_map.active = d2gl::Config::GetBool("mini_map", App.mini_map.active) && ISGLIDE3X();
	App.mini_map.text_over = d2gl::Config::GetBool("mini_map_text_over", App.mini_map.text_over) && ISGLIDE3X();
	App.mini_map.width.value = d2gl::Config::GetInt("mini_map_width", App.mini_map.width.value, App.mini_map.width.min, App.mini_map.width.max);
	App.mini_map.height.value = d2gl::Config::GetInt("mini_map_height", App.mini_map.height.value, App.mini_map.height.min, App.mini_map.height.max);

	App.motion_prediction = d2gl::Config::GetBool("motion_prediction", App.motion_prediction);
	App.skip_intro = d2gl::Config::GetBool("skip_intro", App.skip_intro);
	App.no_pickup = d2gl::Config::GetBool("no_pickup", App.no_pickup);
	App.show_item_quantity = d2gl::Config::GetBool("show_item_quantity", App.show_item_quantity);
	App.show_fps = d2gl::Config::GetBool("show_fps", App.show_fps);

	App.gl_ver.x = d2gl::Config::GetInt("gl_ver_major", App.gl_ver.x, 3, 4);
	App.gl_ver.y = d2gl::Config::GetInt("gl_ver_minor", App.gl_ver.y, 0, 6);
	if (App.gl_ver.x == 3) {
		App.gl_ver.y = 3;
	}

	App.use_compute_shader = d2gl::Config::GetBool("use_compute_shader", App.use_compute_shader);

	App.dlls_early = d2gl::Config::GetString("load_dlls_early", App.dlls_early);
	App.dlls_late = d2gl::Config::GetString("load_dlls_late", App.dlls_late);


	// Setup resolutions
	std::vector<std::pair<uint32_t, uint32_t>> window_sizes = {
		{ 800, 600 },
		{ 960, 720 },
		{ 1024, 768 },
		{ 1200, 900 },
		{ 1280, 960 },
		{ 1440, 1080 },
		{ 1600, 1200 },
		{ 1920, 1440 },
		{ 2560, 1920 },
		{ 2732, 2048 },
		{ 1068, 600 },
		{ 1280, 720 },
		{ 1600, 900 },
		{ 1920, 1080 },
		{ 2048, 1152 },
		{ 2560, 1440 },
		{ 3200, 1800 },
		{ 3840, 2160 },
	};

	int index = 1;
	App.resolutions.items.push_back({ "Custom Size", glm::uvec2(0, 0) });
	for (size_t i = 0; i < window_sizes.size(); i++) {
		const auto& p = window_sizes[i];
		if (App.desktop_resolution.z < p.first || App.desktop_resolution.w < p.second)
			continue;

		char label[50] = { 0 };
		bool is_4x3 = p.first / 4 == p.second / 3;
		sprintf_s(label, 50, "%d x %d (%s)", p.first, p.second, (is_4x3 ? "4:3" : "16:9"));
		App.resolutions.items.push_back({ label, glm::uvec2(p.first, p.second) });

		if (App.window.size.x == p.first && App.window.size.y == p.second)
			App.resolutions.selected = index;
		index++;
	}

	App.presets.items.push_back({ "Ultra", options_preset("Ultra", true, true, 2, true, true, true) });
	App.presets.items.push_back({ "High", options_preset("High", true, true, 1, true, true, true) });
	App.presets.items.push_back({ "Medium", options_preset("Medium", false, true, 0, false, false, true) });
	App.presets.items.push_back({ "Low", options_preset("Low", false, false, 0, false, false, false) });
	App.presets.items.push_back({ "Custom", options_preset("Custom", true, true, 2, true, true, true) });

	if (bCreateFile) {
		SaveConfig();
	}
}
}
