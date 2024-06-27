/*
	D2GL: Diablo 2 LoD Glide/DDraw to OpenGL Wrapper.
	Copyright (C) 2023  Bayaraa

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "pch.h"
#include "menu.h"
#include "d2/common.h"
#include "helpers.h"
#include "modules/hd_text.h"
#include "modules/mini_map.h"
#include "modules/motion_prediction.h"
#include "option/config.h"
#include "win32.h"

namespace d2gl {

#define drawCombo_m(a, b, c, d, e, id)  \
	static int opt_##id = b##.selected; \
	if (drawCombo(a, &b, c, d, &opt_##id, e))

#define drawCheckbox_m(a, b, c, id) \
	static bool opt_##id = b;       \
	if (drawCheckbox(a, &b, c, &opt_##id))

#define drawSlider_m(type, a, b, c, d, id) \
	static type opt_##id = b##.value;      \
	if (drawSlider(#id, a, &b, c, d, &opt_##id))

#define checkChanged(x) \
	if (!changed && x)  \
	changed = true

Menu::Menu()
{
	m_colors[Color::Default] = ImColor(199, 179, 119);
	m_colors[Color::Orange] = ImColor(255, 150, 0);
	m_colors[Color::White] = ImColor(255, 255, 255, 180);
	m_colors[Color::Gray] = ImColor(150, 150, 150);

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImColor(0, 0, 0);
	style.Colors[ImGuiCol_Border] = ImColor(34, 34, 34);
	style.Colors[ImGuiCol_Separator] = ImColor(34, 34, 34);
	style.Colors[ImGuiCol_Text] = m_colors[Color::Default];
	style.Colors[ImGuiCol_TitleBg] = ImColor(0, 0, 0, 240);
	style.Colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0, 240);
	style.Colors[ImGuiCol_Header] = ImColor(25, 25, 25);
	style.Colors[ImGuiCol_HeaderActive] = ImColor(45, 45, 45);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(35, 35, 35);
	style.Colors[ImGuiCol_SliderGrab] = ImColor(60, 60, 60);
	style.Colors[ImGuiCol_SliderGrabActive] = m_colors[Color::Orange];
	style.Colors[ImGuiCol_CheckMark] = m_colors[Color::Orange];
	style.Colors[ImGuiCol_FrameBg] = ImColor(25, 25, 25);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(35, 35, 35);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(45, 45, 45);
	style.Colors[ImGuiCol_Button] = ImColor(25, 25, 25);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(35, 35, 35);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(45, 45, 45);
	style.Colors[ImGuiCol_ScrollbarBg] = ImColor(10, 10, 10);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImColor(25, 25, 25);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(35, 35, 35);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(45, 45, 45);
	style.Colors[ImGuiCol_Tab] = ImColor(30, 30, 30, 150);
	style.Colors[ImGuiCol_TabActive] = ImColor(40, 40, 40);
	style.Colors[ImGuiCol_TabHovered] = ImColor(40, 40, 40, 200);
	style.Colors[ImGuiCol_TabUnfocused] = ImColor(30, 30, 30, 150);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImColor(40, 40, 40);

	style.ScrollbarSize = 10.0f;
	style.ScrollbarRounding = 0.0f;
	style.WindowTitleAlign.x = 0.5f;
	style.WindowPadding = { 16.0f, 16.0f };
	style.WindowBorderSize = 1.0f;
	style.WindowRounding = 0.0f;
	style.ItemInnerSpacing = { 10.0f, 0.0f };
	style.FrameRounding = 2.0f;
	style.FramePadding = { 4.0f, 4.0f };
	style.FrameBorderSize = 1.0f;
	style.ChildRounding = 0.0f;
	style.TabRounding = 2.0f;
	style.PopupRounding = 2.0f;
	style.GrabRounding = 1.0f;
	style.GrabMinSize = 30.0f;
	style.DisabledAlpha = 0.4f;

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = NULL;

	BufferData font1 = helpers::loadFile("assets\\fonts\\ExocetBlizzardMedium.otf");
	BufferData font2 = helpers::loadFile("assets\\fonts\\Formal436BT.ttf");

	io.Fonts->AddFontDefault();
	m_fonts[20] = font1.size ? io.Fonts->AddFontFromMemoryTTF((void*)font1.data, font1.size, 20.0f) : io.Fonts->Fonts[0];
	m_fonts[17] = font1.size ? io.Fonts->AddFontFromMemoryTTF((void*)font1.data, font1.size, 17.0f) : io.Fonts->Fonts[0];
	m_fonts[15] = font1.size ? io.Fonts->AddFontFromMemoryTTF((void*)font1.data, font1.size, 15.0f) : io.Fonts->Fonts[0];
	m_fonts[14] = font2.size ? io.Fonts->AddFontFromMemoryTTF((void*)font2.data, font2.size, 14.0f) : io.Fonts->Fonts[0];
	m_fonts[12] = font2.size ? io.Fonts->AddFontFromMemoryTTF((void*)font2.data, font2.size, 12.0f) : io.Fonts->Fonts[0];

	App.menu_title += (ISGLIDE3X() ? " (Glide / " : " (DDraw / ");
	//App.menu_title += "OpenGL: " + App.gl_ver_str + " / D2LoD: " + helpers::getVersionString() + " / " + helpers::getLangString() + ")";
	App.menu_title += "OpenGL: " + App.gl_ver_str + " / D2VN" " / " + helpers::getLangString() + ")";
}

void Menu::toggle(bool force)
{
	m_visible = force ? true : !m_visible;

	if (m_visible) {
		m_options.vsync = App.vsync;
		m_options.window = App.window;
		m_options.foreground_fps = App.foreground_fps;
		m_options.background_fps = App.background_fps;
		m_options.unlock_cursor = App.cursor.unlock;

		m_closing = false;
	}
}

void Menu::check()
{
	if (m_closing) {
		win32::setCursorLock();
		m_closing = false;
	}

	if (m_changed) {
		App.context->setFpsLimit(!App.vsync && App.foreground_fps.active, App.foreground_fps.range.value);

		win32::setWindowRect();
		win32::windowResize();
		m_changed = false;
	}
}

void Menu::draw()
{
	// Update the config always on exit
	if (m_visible != m_visible_t) {
		if (!m_visible) {
			App.config.SaveConfig();
		}
		m_visible_t = m_visible;
	}

	if (!m_visible) {
		return;
	}


	App.context->imguiStartFrame();
	Menu::updateSelectedQualityPreset();

	ImVec2 window_pos = { (float)App.window.size.x * 0.5f, (float)App.window.size.y * 0.5f };
	ImVec2 max_size = { (float)App.window.size.x - 20.0f, (float)App.window.size.y - 20.0f };
	static ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	static ImGuiCond window_pos_cond = ImGuiCond_Appearing;

	ImGui::SetNextWindowSize({ 680.0f, 600.0f }, ImGuiCond_Always);
	ImGui::SetNextWindowSizeConstraints({ 10.0f, 10.0f }, max_size);
	ImGui::SetNextWindowPos(window_pos, window_pos_cond, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowBgAlpha(0.90f);
	ImGui::PushFont(m_fonts[20]);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10.0f, 10.0f });
	ImGui::Begin(App.menu_title.c_str(), &m_visible, window_flags);
	ImGui::PopStyleVar();
	window_pos_cond = ImGuiCond_Appearing;

	// clang-format off
	const ImVec4 col = ImColor(50, 50, 50);
	static int active_tab = 0;

	ImGui::PushStyleColor(ImGuiCol_Border, col);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 16.0f, 10.0f });
	if (ImGui::BeginTabBar("tabs", ImGuiTabBarFlags_None)) {
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::SetCursorPos({ 530.0f, 74.0f });
		ImGui::PushFont(m_fonts[14]);
		ImGui::PushStyleColor(ImGuiCol_Text, m_colors[Color::Gray]);
		ImGui::Text("D2GL v%s by Bayaraa.", App.version_str.c_str());
		ImGui::PopStyleColor();
		ImGui::PopFont();	
		// ImGui::SetTabItemClosed("Screen");
		if (tabBegin("Screen", 0, &active_tab)) {
			bool changed = m_options.pos_changed;
			childBegin("##w1", true, true);
			drawCombo_m("Quality Preset", App.presets, "Global quality preset, affects performance.", false, 17, presets)
			{
				Menu::applyQualityPreset();
			}
			checkChanged(m_options.vsync != App.vsync);
			drawSeparator();
			ImGui::BeginDisabled(m_options.window.fullscreen);
				drawCombo_m("Window Size", App.resolutions, "", false, 17, resolutions);
				checkChanged(App.resolutions.items[App.resolutions.selected].value != m_options.window.size_save);
				ImGui::Dummy({ 0.0f, 1.0f });
				ImGui::BeginDisabled(App.resolutions.selected);
					drawInput2("##ws", "Input custom width & height. (min: 800 x 600)", (glm::ivec2*)(&m_options.window.size_save), { 800, 600 }, { App.desktop_resolution.z, App.desktop_resolution.w });
					checkChanged(!App.resolutions.selected && App.window.size != m_options.window.size_save);
				ImGui::EndDisabled();
				drawSeparator();
				drawCheckbox_m("Centered Window", m_options.window.centered, "Make window centered to desktop screen.", centered_window);
				checkChanged(m_options.window.centered != App.window.centered);
				ImGui::Dummy({ 0.0f, 2.0f });
				ImGui::BeginDisabled(m_options.window.centered);
					drawInput2("##wp", "Window position from top left corner.", &m_options.window.position, { App.desktop_resolution.x, App.desktop_resolution.y }, { App.desktop_resolution.z, App.desktop_resolution.w });
					checkChanged(!m_options.window.centered && App.window.position != m_options.window.position);
				ImGui::EndDisabled();
			ImGui::EndDisabled();
			drawSeparator();
			drawCheckbox_m("Unlock Cursor", m_options.unlock_cursor, "Cursor will not locked within window.", unlock_cursor);
			checkChanged(m_options.unlock_cursor != App.cursor.unlock);
			childSeparator("##w2", true);
			drawCheckbox_m("Fullscreen", m_options.window.fullscreen, "Game will run in windowed mode if unchecked.", fullscreen);
			checkChanged(m_options.window.fullscreen != App.window.fullscreen);
			drawSeparator();
			drawCheckbox_m("V-Sync", m_options.vsync, "Vertical Synchronization.", vsync);
			checkChanged(m_options.vsync != App.vsync);
			drawSeparator();
			ImGui::BeginDisabled(m_options.vsync);
				drawCheckbox_m("Max Foreground FPS", m_options.foreground_fps.active, "", foreground_fps);
				checkChanged(!m_options.vsync && m_options.foreground_fps.active != App.foreground_fps.active);
				ImGui::BeginDisabled(!m_options.foreground_fps.active);
					drawSlider_m(int, "", m_options.foreground_fps.range, "%d", "Max fps when game window is active.", foreground_fps_val);
					checkChanged(!m_options.vsync && m_options.foreground_fps.active && m_options.foreground_fps.range.value != App.foreground_fps.range.value);
				ImGui::EndDisabled();
			ImGui::EndDisabled();
			drawSeparator();
			drawCheckbox_m("Max Background FPS", m_options.background_fps.active, "", background_fps);
			checkChanged(m_options.background_fps.active != App.background_fps.active);
			ImGui::BeginDisabled(!m_options.background_fps.active);
				drawSlider_m(int, "", m_options.background_fps.range, "%d", "Max fps when game window is in inactive.", background_fps_val);
				checkChanged(m_options.background_fps.active && m_options.background_fps.range.value != App.background_fps.range.value);
			ImGui::EndDisabled();
			drawSeparator();
			drawCheckbox_m("Auto Minimize", m_options.window.auto_minimize, "Auto minimize when lose focus while in fullscreen.", auto_minimize);
			checkChanged(m_options.window.auto_minimize != App.window.auto_minimize);
			drawSeparator();
			drawCheckbox_m("Dark Mode", m_options.window.dark_mode, "Dark window title bar. Affect on next launch.", dark_mode);
			checkChanged(m_options.window.dark_mode != App.window.dark_mode);
			childEnd();
			ImGui::BeginDisabled(!changed);
				if (drawNav("Apply Changes")) {
					if (App.resolutions.selected) {
						const auto val = App.resolutions.items[App.resolutions.selected].value;
						m_options.window.size_save = val;
					}

					if (App.window.size != m_options.window.size_save || App.window.fullscreen != m_options.window.fullscreen)
						window_pos_cond = ImGuiCond_Always;

					App.window.fullscreen = m_options.window.fullscreen;
					App.window.size = m_options.window.size_save;
					App.window.size_save = m_options.window.size_save;
					App.window.centered = m_options.window.centered;
					App.window.position = m_options.window.position;
					App.cursor.unlock = m_options.unlock_cursor;
					App.window.auto_minimize = m_options.window.auto_minimize;
					App.window.dark_mode = m_options.window.dark_mode;
					App.vsync = m_options.vsync;
					App.foreground_fps = m_options.foreground_fps;
					App.background_fps = m_options.background_fps;

					App.config.SaveConfig();

					m_options.pos_changed = false;
					m_changed = true;
				}
			ImGui::EndDisabled();
			tabEnd();
		}
		if (tabBegin("Graphics", 1, &active_tab)) {
			drawCombo_m("Upscale Shader Preset", App.shader.presets, "", true, 17, shader_preset);
			ImGui::SameLine(0.0f, 4.0f);
			ImGui::BeginDisabled(App.shader.selected == App.shader.presets.selected);
				if (drawButton("Apply", { 100.0f, 0.0f }))
					App.shader.selected = App.shader.presets.selected;
			ImGui::EndDisabled();
			drawDescription("RetroArch's slang shader preset files (.slangp).", m_colors[Color::Gray]);
			childBegin("##w3", true);
			drawSeparator();
			drawCheckbox_m("Luma Sharpen", App.sharpen.active, "", sharpen);
			ImGui::BeginDisabled(!App.sharpen.active);
			drawSlider_m(float, "", App.sharpen.strength, "%.3f", "", sharpen_strength);
			drawDescription("Strength of the sharpening.", m_colors[Color::Gray], 12);
			drawSlider_m(float, "", App.sharpen.clamp, "%.3f", "", sharpen_clamp);
			drawDescription("Limit maximum amount of sharpening pixel.", m_colors[Color::Gray], 12);
			drawSlider_m(float, "", App.sharpen.radius, "%.3f", "", sharpen_radius);
			drawDescription("Radius of the sampling pattern.", m_colors[Color::Gray], 12);
			ImGui::EndDisabled();
			drawSeparator();
			drawCheckbox_m("FXAA", App.fxaa.active, "Fast approximate anti-aliasing.", fxaa);
			ImGui::BeginDisabled(!App.fxaa.active);
				drawCombo_m("", App.fxaa.presets, "FXAA quality presets.", false, 17, fxaa_preset);
			ImGui::EndDisabled();
			childSeparator("##w4");
			drawSeparator();
			ImGui::BeginDisabled(!ISGLIDE3X());
				drawCombo_m("Color Grading", App.lut, "Lookup table (LUT).", false, 17, lut);
				drawSeparator();
				drawCheckbox_m("Bloom Effect", App.bloom.active, "", bloom);
				ImGui::BeginDisabled(!App.bloom.active);
					drawSlider_m(float, "", App.bloom.exposure, "%.3f", "", bloom_exposure);
					drawDescription("Bloom exposure setting.", m_colors[Color::Gray], 12);
					drawSlider_m(float, "", App.bloom.gamma, "%.3f", "", bloom_gamma);
					drawDescription("Bloom Gamma setting.", m_colors[Color::Gray], 12);
				ImGui::EndDisabled();
			ImGui::EndDisabled();
			drawSeparator();
			drawLabel("Stretched Viewport", m_colors[Color::Orange]);
			drawCheckbox_m("Horizontal", App.viewport.stretched.x, "", stretched_horizontal)
			{
				App.context->getCommandBuffer()->resize();
			}
			ImGui::SameLine(150.0f);
			drawCheckbox_m("Vertical", App.viewport.stretched.y, "", stretched_vertical)
			{
				App.context->getCommandBuffer()->resize();
			}
			drawDescription("Stretch viewport to window size.", m_colors[Color::Gray], 14);
			childEnd();
			tabEnd();
		}
		if (tabBegin("Features", 2, &active_tab)) {
			childBegin("##w5", true);
			drawCheckbox_m("HD Cursor", App.hd_cursor, "High-definition in game & menu screen cursor.", hd_cursor);
			drawSeparator();
			drawCheckbox_m("HD Text", App.hd_text.active, "High-definition ingame texts.", hd_text)
			{
				d2::patch_hd_text->toggle(App.hd_text.active);
			}
			ImGui::BeginDisabled(!App.hd_text.active || ISHDTEXT());
				drawSlider_m(float, "", App.hd_text.scale, "%.3f", "Global scale for HD text.", hd_text_scale);
				if (App.hd_text.active)
					modules::HDText::Instance().updateFontSize();
			ImGui::EndDisabled();
			drawSeparator();
			ImGui::BeginDisabled(!ISGLIDE3X() || !App.mini_map.available);
				drawCheckbox_m("Mini Map", App.mini_map.active, "Always on minimap widget.", mini_map)
				{
					d2::patch_minimap->toggle(App.mini_map.active);
				}
				ImGui::BeginDisabled(!App.mini_map.active);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::SameLine(20.0f);
					drawCheckbox_m("Text over map", App.mini_map.text_over, "", mini_map_text_over);
					ImGui::Dummy({ 0.0f, 2.0f });
					drawSlider_m(int, "", App.mini_map.width, "%d", "Minimap width.", mini_map_width_val);
					drawSlider_m(int, "", App.mini_map.height, "%d", "Minimap height.", mini_map_height_val);
					if (App.mini_map.active)
						modules::MiniMap::Instance().resize();
				ImGui::EndDisabled();
			ImGui::EndDisabled();
			/*ImGui::BeginDisabled(true);
				drawCheckbox_m("HD Orbs", App.hd_orbs.active, "High-definition life & mana orbs. (coming soon)", hd_orbs)
					saveBool("Feature", "hd_orbs", App.hd_orbs.active);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SameLine(36.0f);
				ImGui::BeginDisabled(!App.hd_orbs.active);
					drawCheckbox_m("Centered", App.hd_orbs.centered, "", hd_orbs_centered)
						saveBool("Feature", "hd_orbs_centered", App.hd_orbs.centered);
				ImGui::EndDisabled();
			ImGui::EndDisabled();*/
			childSeparator("##w6");
			drawCheckbox_m("Motion Prediction", App.motion_prediction, "D2DX's motion prediction feature.", motion_prediction)
			{
				modules::MotionPrediction::Instance().toggle(App.motion_prediction);
			}
			drawSeparator();
			drawCheckbox_m("Skip Intro", App.skip_intro, "Auto skip intro videos on launch.", skip_intro);
			drawSeparator();
			drawCheckbox_m("No Pickup", App.no_pickup, "Auto /nopickup option on launch (exclude 1.09d).", no_pickup);
			drawSeparator();
			drawCheckbox_m("Show Monster Resistances", App.show_monster_res, "Show monster resistances.", show_monster_res);
			ImGui::BeginDisabled(!App.show_monster_res);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SameLine(20.0f);
				drawCheckbox_m("Show Monster Resist In HP Bar", App.show_monster_res_in_mon_hp_bar, "Show monster resistances in hp bar.", show_monster_res_in_mon_hp_bar);
			ImGui::EndDisabled();
			drawSeparator();
			drawCheckbox_m("Show FPS", App.show_fps, "FPS Counter on bottom center.", show_fps);
			drawSeparator();
			drawCheckbox_m("Show Item Color Background", App.show_item_color_bg, "Show Item Color Background", show_item_color_bg);
			//ImGui::BeginDisabled(true);
			//drawCheckbox_m("Show Item Quantity", App.show_item_quantity, "Show item quantity on bottom left corner of icon.", show_item_quantity);
			//ImGui::EndDisabled();
			//drawSeparator();
			childEnd();
			tabEnd();
		}
#ifdef _HDTEXT
		if (tabBegin("HD Text", 3, &active_tab)) {
			ImGuiIO& io = ImGui::GetIO();
			ImGui::PushFont(io.Fonts->Fonts[0]);
			m_ignore_font = true;
			drawCombo_m("", App.hdt.fonts, "", "", 17, hdt_fonts)
			{
				const auto font_id = (uint32_t)App.hdt.fonts.items[App.hdt.fonts.selected].value;
				modules::HDText::Instance().getFont(font_id)->getMetrics();
			}
			drawSeparator();
			childBegin("##hdt0", true);
			drawSlider_m(float, "Font Size", App.hdt.size, "%.3f", "", hdt_size);
			drawSlider_m(float, "Font Weight", App.hdt.weight, "%.3f", "", hdt_weight);
			drawSlider_m(float, "Letter Spacing", App.hdt.letter_spacing, "%.3f", "", hdt_letter_spacing);
			drawSlider_m(float, "Line Height", App.hdt.line_height, "%.3f", "", hdt_line_height);
			drawSlider_m(float, "Shadow Intensity", App.hdt.shadow_intensity, "%.3f", "", hdt_shadow_intensity);
			drawSlider_m(float, "Text Offset (X Coordinate)", App.hdt.offset_x, "%.3f", "", hdt_offset_x);
			drawSlider_m(float, "Text Offset (Y Coordinate)", App.hdt.offset_y, "%.3f", "", hdt_offset_y);
			drawSlider_m(float, "Symbol Offset (Y Coordinate)", App.hdt.symbol_offset, "%.3f", "", hdt_symbol_offset);
			childSeparator("##hdt1");
			ImGui::BeginDisabled(App.game.screen != GameScreen::InGame);
			drawCheckbox_m("Show sample text", App.hdt.show_sample, "", hdt_show_sample);
			ImGui::EndDisabled();
			drawSeparator();
			const auto font_id = (uint32_t)App.hdt.fonts.items[App.hdt.fonts.selected].value;
			const auto font = modules::HDText::Instance().getFont(font_id);
			font->updateMetrics();
			char result[2100] = { "" };
			const auto& str = modules::HDText::Instance().getAllFontMetricString();
			strcpy_s(result, str.c_str());
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 8.0f, 7.0f });
			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
			ImGui::InputTextMultiline("##result", result, IM_ARRAYSIZE(result), { 0, 249 }, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoHorizontalScroll);
			ImGui::PopStyleVar();
			if (ImGui::Button("Copy Text"))
				ImGui::SetClipboardText(result);
			childEnd();
			m_ignore_font = false;
			ImGui::PopFont();
			tabEnd();
		}
#endif
#ifdef _DEBUG
		if (tabBegin("Debug", 3, &active_tab)) {
			ImGuiIO& io = ImGui::GetIO();
			ImGui::PushFont(io.Fonts->Fonts[0]);
			ImGui::Checkbox("Check6", (bool*)(&App.var[6]));
			ImGui::PopFont();
			tabEnd();
		}
#endif
		ImGui::EndTabBar();
	}
	ImGui::PopFont();
	//if (active_tab != 3) {
	//	ImGui::SetCursorPos({ 16.0f, 500.0f });
	//	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });
	//	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
	//	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
	//	ImGui::BeginChildFrame(ImGui::GetID("#wiki"), { 300.0f, 24.0f }, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
	//	ImGui::PopStyleVar(3);
	//	ImGui::PushFont(m_fonts[15]);
	//	if (ImGui::Button(" Open Configuration Wiki Page > "))
	//		ShellExecute(0, 0, L"https://github.com/bayaraa/d2gl/wiki/Configuration", 0, 0, SW_SHOW);
	//	ImGui::PopFont();
	//	ImGui::EndChildFrame();
	//}
	ImGui::End();

	// clang-format on
	if (!m_visible)
		m_closing = true;

	App.context->imguiRender();
}

void Menu::updateSelectedQualityPreset()
{
	int i = 0;
	int customIndex = 0;

	for (auto preset : App.presets.items) {
		if (App.bloom.active == preset.value.enable_bloom &&
			App.fxaa.active == preset.value.enable_fxaa &&
			App.fxaa.presets.selected == preset.value.fxaa_quality &&
			App.sharpen.active == preset.value.enable_sharpen &&
			m_options.vsync == preset.value.enable_vsync &&
			App.motion_prediction == preset.value.enable_motion_prediction)
		{
			App.presets.selected = i;
			return;
		}
		if (preset.name.compare("Custom") == 0) {
			customIndex = i;
		}
		i++;
	}

	App.presets.selected = customIndex;
}

void Menu::applyQualityPreset()
{
	const d2gl::options_preset val = App.presets.items[App.presets.selected].value;

	if (val.preset_name.compare("Custom") == 0) {
		return;
	}

	App.bloom.active = val.enable_bloom;
	App.fxaa.active = val.enable_fxaa;
	App.fxaa.presets.selected = val.fxaa_quality;
	App.sharpen.active = val.enable_sharpen;
	m_options.vsync = val.enable_vsync;
	App.motion_prediction = val.enable_motion_prediction;
}

bool Menu::tabBegin(const char* title, int tab_num, int* active_tab)
{
	static const ImVec4 inactive_col = ImColor(100, 100, 100, 200);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 16.0f, 10.0f });
	ImGui::PushStyleColor(ImGuiCol_Text, tab_num == *active_tab ? m_colors[Color::Orange] : inactive_col);
	bool ret = ImGui::BeginTabItem(title);
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	if (ret) {
		*active_tab = tab_num;
		drawSeparator(2.0f, 0.0f);
	}
	return ret;
}

void Menu::tabEnd()
{
	ImGui::EndTabItem();
}

void Menu::childBegin(const char* id, bool half_width, bool with_nav)
{
	ImVec2 size = { 0.0f, with_nav ? -60.0f : 0.0f };
	if (half_width)
		size.x = ImGui::GetWindowContentRegionWidth() / 2.0f - 8.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
	ImGui::BeginChildFrame(ImGui::GetID(id), size, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
	ImGui::PopStyleVar(3);
}

void Menu::childSeparator(const char* id, bool with_nav)
{
	childEnd();
	ImGui::SameLine(0.0f, 16.0f);
	childBegin(id, true, with_nav);
}

void Menu::childEnd()
{
	ImGui::EndChildFrame();
}

bool Menu::drawNav(const char* btn_label)
{
	static bool ret = false;
	const ImVec4 sep_col = ImColor(40, 40, 40);
	ImGui::PushStyleColor(ImGuiCol_Separator, sep_col);
	ImGui::PushStyleColor(ImGuiCol_Text, m_colors[Color::Orange]);
	drawSeparator(6.0f);
	ImGui::Dummy({ ImGui::GetWindowContentRegionWidth() - 200.0f, 0.0f });
	ImGui::SameLine();
	ret = ImGui::Button(btn_label, { 192.0f, 36.0f });
	ImGui::PopStyleColor(2);
	return ret;
}

bool Menu::drawCheckbox(const char* title, bool* option, const char* desc, bool* opt)
{
	ImGui::PushStyleColor(ImGuiCol_Text, m_colors[Color::Orange]);
	if (!m_ignore_font)
		ImGui::PushFont(m_fonts[17]);
	ImGui::Checkbox(title, option);
	if (!m_ignore_font)
		ImGui::PopFont();
	ImGui::PopStyleColor();
	drawDescription(desc, m_colors[Color::Gray]);

	if (*option != *opt) {
		*opt = *option;
		return true;
	}
	return false;
}

template <typename T>
bool Menu::drawCombo(const char* title, Select<T>* select, const char* desc, bool have_btn, int* opt, int size)
{
	bool ret = false;

	drawLabel(title, m_colors[Color::Orange], 17);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 8.0f, 5.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 8.0f });
	if (!m_ignore_font)
		ImGui::PushFont(m_fonts[size]);
	ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (have_btn ? 104.0f : 0.0f));
	auto& selected = select->items[select->selected];
	if (ImGui::BeginCombo(("##" + std::string(title)).c_str(), selected.name.c_str(), ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_HeightLargest)) {
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 8.0f, 8.0f });
		for (size_t i = 0; i < select->items.size(); i++) {
			const bool is_selected = (select->selected == i);
			if (is_selected)
				ImGui::PushStyleColor(ImGuiCol_Text, m_colors[Color::Orange]);
			if (ImGui::Selectable(select->items[i].name.c_str(), is_selected)) {
				select->selected = i;
				if (select->selected != *opt) {
					*opt = select->selected;
					ret = true;
				}
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
				ImGui::PopStyleColor();
			}
		}
		ImGui::PopStyleVar();
		ImGui::EndCombo();
	}
	if (!m_ignore_font)
		ImGui::PopFont();
	ImGui::PopStyleVar(2);
	drawDescription(desc, m_colors[Color::Gray]);
	return ret;
}

void Menu::drawInput2(const std::string& id, const char* desc, glm::ivec2* input, glm::ivec2 min, glm::ivec2 max)
{
	float width = ImGui::GetWindowContentRegionWidth() / 2 - 2.0f;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 8.0f, 5.0f });
	ImGui::PushFont(m_fonts[17]);
	ImGui::SetNextItemWidth(width);
	ImGui::InputInt((id + "x").c_str(), &input->x, 0, 0);
	ImGui::SameLine(0.0f, 4.0f);
	ImGui::SetNextItemWidth(width);
	ImGui::InputInt((id + "y").c_str(), &input->y, 0, 0);
	ImGui::PopFont();
	ImGui::PopStyleVar();
	drawDescription(desc, m_colors[Color::Gray]);

	if (input->x < min.x)
		input->x = min.x;
	if (input->x > max.x)
		input->x = max.x;

	if (input->y < min.y)
		input->y = min.y;
	if (input->y > max.y)
		input->y = max.y;
}

template <typename T>
bool Menu::drawSlider(const std::string& id, const char* title, Range<T>* range, const char* format, const char* desc, T* opt)
{
	drawLabel(title, m_colors[Color::Orange]);
	ImGuiDataType type = strcmp(typeid(T).name(), "int") == 0 ? ImGuiDataType_S32 : ImGuiDataType_Float;
	ImGui::SetNextItemWidth(80.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 3.0f });
	if (!m_ignore_font)
		ImGui::PushFont(m_fonts[15]);
	ImGui::InputScalar(("##" + id).c_str(), type, &range->value, 0, 0, format);
	ImGui::SameLine(0.0f, 4.0f);
	ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - 84.0f);
	ImGui::SliderScalar(id.c_str(), type, &range->value, &range->min, &range->max, "", ImGuiSliderFlags_NoInput);
	if (!m_ignore_font)
		ImGui::PopFont();
	ImGui::PopStyleVar();
	drawDescription(desc, m_colors[Color::Gray]);

	if (range->value < range->min)
		range->value = range->min;
	if (range->value > range->max)
		range->value = range->max;

	if (range->value != *opt && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		*opt = range->value;
		return true;
	}
	return false;
}

bool Menu::drawButton(const char* label, const ImVec2& btn_size, int size)
{
	bool ret = false;

	ImGui::PushStyleColor(ImGuiCol_Text, m_colors[Color::Orange]);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 8.0f, 5.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 8.0f });
	if (!m_ignore_font)
		ImGui::PushFont(m_fonts[size]);
	if (ImGui::Button(label, btn_size))
		ret = true;
	if (!m_ignore_font)
		ImGui::PopFont();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor();
	return ret;
}

void Menu::drawSeparator(float y_padd, float alpha)
{
	ImGui::Dummy({ 0.0f, y_padd - 1.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
	ImGui::Separator();
	ImGui::PopStyleVar(2);
	ImGui::Dummy({ 0.0f, y_padd });
}

void Menu::drawLabel(const char* title, const ImVec4& color, int size)
{
	if (!title || !title[0] || title[0] == ' ')
		return;

	if (!m_ignore_font)
		ImGui::PushFont(m_fonts[size]);
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::Text(title);
	ImGui::PopStyleColor();
	if (!m_ignore_font)
		ImGui::PopFont();
}

void Menu::drawDescription(const char* desc, const ImVec4& color, int size)
{
	if (!desc || !desc[0])
		return;

	if (!m_ignore_font)
		ImGui::PushFont(m_fonts[size]);
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::Text(desc);
	ImGui::PopStyleColor();
	if (!m_ignore_font)
		ImGui::PopFont();
}

}