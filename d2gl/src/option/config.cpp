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
	yaml["window_dark_mode"] = App.window.dark_mode;
	yaml["window_vsync"] = App.vsync;
	yaml["window_centered"] = App.window.centered;
	yaml["App.window.position.x"] = App.window.position.x;
	yaml["App.window.position.y"] = App.window.position.y;
	yaml["foreground_fps_active"] = App.foreground_fps.active;
	yaml["foreground_fps_value"] = App.foreground_fps.range.value;
	yaml["background_fps_active"] = App.background_fps.active;
	yaml["background_fps_value"] = App.background_fps.range.value;
	yaml["shader"] = App.shader.selected;
	yaml["lut"] = App.lut.selected;
	yaml["fxaa"] = App.fxaa;
	yaml["sharpen"] = App.sharpen.active;
	yaml["sharpen_strength"] = App.sharpen.strength.value;
	yaml["sharpen_clamp"] = App.sharpen.clamp.value;
	yaml["sharpen_radius"] = App.sharpen.radius.value;
	yaml["bloom"] = App.bloom.active;
	yaml["bloom_exposure"] = App.bloom.exposure.value;
	yaml["bloom_gamma"] = App.bloom.gamma.value;
	yaml["hd_cursor"] = App.hd_cursor;
	yaml["hd_text"] = App.hd_text;
	yaml["hd_orbs"] = App.hd_orbs.active;
	yaml["hd_orbs_centered"] = App.hd_orbs.centered;
	yaml["mini_map"] = App.mini_map.active;
	yaml["mini_map_text_below"] = App.mini_map.text_below;
	yaml["motion_prediction"] = App.motion_prediction;
	yaml["skip_intro"] = App.skip_intro;
	yaml["no_pickup"] = App.no_pickup;
	yaml["show_fps"] = App.show_fps;
	yaml["gl_ver_major"] = (int)App.gl_ver_major;
	yaml["gl_ver_minor"] = (int)App.gl_ver_minor;
	yaml["use_compute_shader"] = App.use_compute_shader;
	yaml["load_dlls_early"] = App.dlls_early;
	yaml["load_dlls_late"] = App.dlls_late;

	std::ofstream fout(App.yaml_file);
	fout << yaml;
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

	// Setup Shaders
	for (auto& shader : g_shader_upscale) {
		App.shader.items.push_back({ shader.name });
	}

	// Resolution
	App.desktop_resolution = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
	App.window.size = App.desktop_resolution;
	App.window.size.x = d2gl::Config::GetInt("window_size_width", App.desktop_resolution.x, 800, App.desktop_resolution.x);
	App.window.size.y = d2gl::Config::GetInt("window_size_height", App.desktop_resolution.y, 600, App.desktop_resolution.y);
	App.window.size_save = App.window.size;

	App.window.fullscreen = d2gl::Config::GetBool("window_fullscreen", true);
	App.window.dark_mode = d2gl::Config::GetBool("window_dark_mode", true);
	App.vsync = d2gl::Config::GetBool("window_vsync", true);
	App.window.centered = d2gl::Config::GetBool("window_centered", true);

	// Window Position
	App.window.position = { 0, 0 };
	App.window.position.x = d2gl::Config::GetInt("window_position_x", 0, 0, App.desktop_resolution.x);
	App.window.position.y = d2gl::Config::GetInt("window_position_y", 0, 0, App.desktop_resolution.y);

	App.foreground_fps.active = d2gl::Config::GetBool("foreground_fps_active", false);
	App.foreground_fps.range.value = d2gl::Config::GetInt("foreground_fps_value", App.foreground_fps.range.value, App.foreground_fps.range.min, App.foreground_fps.range.max);
	App.background_fps.active = d2gl::Config::GetBool("background_fps_active", true);
	App.background_fps.range.value = d2gl::Config::GetInt("background_fps_value", App.background_fps.range.value, App.background_fps.range.min, App.background_fps.range.max);

	App.shader.selected = d2gl::Config::GetInt("shader", 1, 0, App.shader.items.size() - 1);

	App.lut.selected = d2gl::Config::GetInt("lut", App.lut.selected, 0, App.lut.items.size() - 1);
	App.fxaa = d2gl::Config::GetBool("fxaa", App.fxaa);

	App.sharpen.active = d2gl::Config::GetBool("sharpen", App.sharpen.active);
	App.sharpen.strength.value = d2gl::Config::GetFloat("sharpen_strength", App.sharpen.strength.value, App.sharpen.strength.min, App.sharpen.strength.max);
	App.sharpen.clamp.value = d2gl::Config::GetFloat("sharpen_clamp", App.sharpen.clamp.value, App.sharpen.clamp.min, App.sharpen.clamp.max);
	App.sharpen.radius.value = d2gl::Config::GetFloat("sharpen_radius", App.sharpen.radius.value, App.sharpen.radius.min, App.sharpen.radius.max);

	App.bloom.active = d2gl::Config::GetBool("bloom", App.bloom.active);
	App.bloom.exposure.value = d2gl::Config::GetFloat("bloom_exposure", App.bloom.exposure.value, App.bloom.exposure.min, App.bloom.exposure.max);
	App.bloom.gamma.value = d2gl::Config::GetFloat("bloom_gamma", App.bloom.gamma.value, App.bloom.gamma.min, App.bloom.gamma.max);

	App.hd_cursor = d2gl::Config::GetBool("hd_cursor", App.hd_cursor);
	App.hd_text = d2gl::Config::GetBool("hd_text", App.hd_text);
	App.hd_orbs.active = d2gl::Config::GetBool("hd_orbs", App.hd_orbs.active);
	App.hd_orbs.centered = d2gl::Config::GetBool("hd_orbs_centered", App.hd_orbs.centered);
	App.mini_map.active = d2gl::Config::GetBool("mini_map", App.mini_map.active) && App.api == Api::Glide;
	App.mini_map.text_below = d2gl::Config::GetBool("mini_map_text_below", App.mini_map.text_below) && App.api == Api::Glide;

	App.motion_prediction = d2gl::Config::GetBool("motion_prediction", App.motion_prediction);
	App.skip_intro = d2gl::Config::GetBool("skip_intro", App.skip_intro);
	App.no_pickup = d2gl::Config::GetBool("no_pickup", App.no_pickup);
	App.show_fps = d2gl::Config::GetBool("show_fps", App.show_fps);

	App.gl_ver_major = d2gl::Config::GetInt("gl_ver_major", App.gl_ver_major, 3, 4);
	App.gl_ver_minor = d2gl::Config::GetInt("gl_ver_minor", App.gl_ver_minor, 0, 6);
	if (App.gl_ver_major == 3) {
		App.gl_ver_minor = 3;
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
		if (App.desktop_resolution.x <= p.first || App.desktop_resolution.y <= p.second)
			continue;

		char label[50] = { 0 };
		bool is_4x3 = p.first / 4 == p.second / 3;
		sprintf_s(label, 50, "%d x %d (%s)", p.first, p.second, (is_4x3 ? "4:3" : "16:9"));
		App.resolutions.items.push_back({ label, glm::uvec2(p.first, p.second) });

		if (App.window.size.x == p.first && App.window.size.y == p.second)
			App.resolutions.selected = index;
		index++;
	}

	if (bCreateFile) {
		SaveConfig();
	}
}
}
