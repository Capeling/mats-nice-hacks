#include "menu.hpp"
#include <imgui-hook.hpp>
#include <imgui-hook/imgui/misc/cpp/imgui_stdlib.h>
#include "state.hpp"
#include <MinHook.h>
#include "utils.hpp"
#include "gd.hpp"
#include "explorer.hpp"
#include "Icons.hpp"

void update_speed_hack() {
	const auto value = state().speed_hack_enabled ? state().speed : 1.f;
	if (auto fme = gd::FMODAudioEngine::sharedState())
		if (auto sound = fme->currentSound())
			sound->setPitch(value);
	CCDirector::sharedDirector()->m_pScheduler->setTimeScale(value);
}

void update_fps_bypass() {
	const auto value = state().fps_bypass ? state().fps : 60.f;
	static const auto addr = cocos_symbol<&CCApplication::setAnimationInterval>("?setAnimationInterval@CCApplication@cocos2d@@UAEXN@Z");
	addr(CCApplication::sharedApplication(), 1.0 / value);
	CCDirector::sharedDirector()->setAnimationInterval(1.0 / value);
}

void imgui_render() {
	const bool force = state().just_loaded;
	const auto& font = ImGui::GetIO().Fonts->Fonts.back();
	ImGui::PushFont(font);
	if (state().visible || force) {

		// ImGui::ShowDemoWindow();
		auto frame_size = CCDirector::sharedDirector()->getOpenGLView()->getFrameSize();

		constexpr float border = 25;
		ImGui::SetNextWindowPos({ border, border });
		ImGui::SetNextWindowSizeConstraints({0, 0}, {frame_size.width, frame_size.height - border * 2.f});

		if (ImGui::Begin("mat's nice hacks | capeling fork", nullptr,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar)) {
			if (ImGui::BeginMenuBar()) {
				if (ImGui::MenuItem("Explorer"))
					state().explorer_enabled = !state().explorer_enabled;
				ImGui::EndMenuBar();
			}

			ImGui::SetNextItemWidth(120);
			if (ImGui::DragFloat("##speedhack", &state().speed, 0.05f, 0.001f, 10.f))
				update_speed_hack();
			ImGui::SameLine();
			if (ImGui::Checkbox("Speedhack", &state().speed_hack_enabled))
				update_speed_hack();

			if(ImGui::TreeNode("Bypass")) {
				ImGui::SetNextItemWidth(120);
				if (ImGui::InputFloat("##fpsbypass", &state().fps))
					update_fps_bypass();
				ImGui::SameLine();
				if (ImGui::Checkbox("FPS bypass", &state().fps_bypass))
					update_fps_bypass();

				if (ImGui::Checkbox("No transition", &state().no_transition) || force) {
					// patch
					//   movss dword ptr [esi + 0xf0], xmm0
					// to
					//   xor eax, eax
					//   mov dword ptr [esi + 0xf0], eax
					patchHelp::patch_toggle(gd::cocos_base + 0xa49a7, { 0x31, 0xc0, 0x89, 0x86, 0xf0, 0x00, 0x00, 0x00 }, state().no_transition);
				}
				if (ImGui::Checkbox("Copy hack", &state().copy_hack) || force) {
					// LevelInfoLayer::init and LevelInfoLayer::tryClone
					patchHelp::patch_toggle(gd::base + 0x9c7ed, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, state().copy_hack);
					patchHelp::patch_toggle(gd::base + 0x9dfe5, { 0x90, 0x90 }, state().copy_hack);
				}
				if (ImGui::Checkbox("Edit level", &state().edit_level) || force) {
					// PauseLayer::init
					patchHelp::patch_toggle(gd::base + 0xd62ef, { 0x90, 0x90 }, state().edit_level);
				}
				if (ImGui::Checkbox("Verify hack", &state().verify_hack) || force) {
					// EditLevelLayer::onShare
					patchHelp::patch_toggle(gd::base + 0x3d760, { 0xeb }, state().verify_hack);
				}
				ImGui::Checkbox("Character limit bypass", &state().no_char_limit);
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("Level")) {
			if (ImGui::Checkbox("No particles", &state().no_particles) || force) {
				// CCParticleSystemQuad::draw
				patchHelp::patch_toggle(gd::cocos_base + 0xb77f0, { 0xc3 }, state().no_particles);
			}
				ImGui::Checkbox("Retry keybind (R)", &state().has_retry_keybind);
				if (ImGui::Checkbox("Hide practice buttons", &state().hide_practice) || force) {
					// UILayer::init, replaces two calls with add esp, 4
					patchHelp::patch_toggle(gd::base + 0xfee29, { 0x83, 0xc4, 0x04, 0x90, 0x90, 0x90 }, state().hide_practice);
					patchHelp::patch_toggle(gd::base + 0xfee6a, { 0x83, 0xc4, 0x04, 0x90, 0x90, 0x90 }, state().hide_practice);
				}
				ImGui::Checkbox("Show percent", &state().show_percent);
				ImGui::Checkbox("Hide attempts", &state().hide_attempts);
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("Player")) {

				if (ImGui::Checkbox("No trail", &state().no_trail) || force) {
					// CCMotionStreak::draw
					patchHelp::patch_toggle(gd::cocos_base + 0xac080, { 0xc3 }, state().no_trail);
				}
				if (ImGui::Checkbox("No wave trail", &state().no_wave_trail) || force) {
					// PlayerObject::activateStreak
					patchHelp::patch_toggle(gd::base + 0xe0d54, { 0xeb }, state().no_wave_trail);
				}
				if (ImGui::Checkbox("Solid wave trail", &state().solid_wave_trail) || force) {
					// PlayerObject::setupStreak
					patchHelp::patch_toggle(gd::base + 0xd9ade, { 0x90, 0x90 }, state().solid_wave_trail);
				}
				ImGui::Checkbox("No death effect", &state().no_death_effect);
				ImGui::Checkbox("Hide player", &state().hide_player);
				ImGui::SetNextItemWidth(120.f);
				ImGui::InputFloat("##rainbowiconspeed", &state().rainbow_speed);
				ImGui::SameLine();
				ImGui::Checkbox("Rainbow icon", &state().rainbow_color);
				ImGui::Checkbox("Use mini icon", &state().use_mini_icon);
				ImGui::Checkbox("No mini icon", &state().no_mini_icon);
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("Editor")) {
				ImGui::Checkbox("Editor preview mode", &state().preview_mode);
				if (ImGui::Checkbox("Hide trigger lines", &state().hide_trigger_lines) || force) {
					// DrawGridLayer::draw
					patchHelp::patch_toggle(gd::base + 0x93e08, { 0xE9, 0xCE, 0x00, 0x00, 0x00, 0x90 }, state().hide_trigger_lines);
				}
				if (ImGui::Checkbox("Hide grid", &state().hide_grid) || force) {
					// DrawGridLayer::draw
					// gets rid of the line at the start and the ground line but oh well 
					// setting the opacity to 0 didnt work if u zoomed in it was weird
					patchHelp::patch_toggle(gd::base + 0x938a0, { 0xe9, 0x5a, 0x01, 0x00, 0x00, 0x90 }, state().hide_grid);
					patchHelp::patch_toggle(gd::base + 0x93a4a, { 0xe9, 0x54, 0x01, 0x00, 0x00, 0x90 }, state().hide_grid);
				}
				if (ImGui::Checkbox("Smooth editor trail", &state().smooth_editor_trail) || force) {
					// LevelEditorLayer::update
					patchHelp::patch_toggle(gd::base + 0x91a34, { 0x90, 0x90 }, state().smooth_editor_trail);
				}
				ImGui::Checkbox("Always fix yellow color bug", &state().always_fix_hue);
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("Status labels")) {
				ImGui::SetNextItemWidth(60);
				ImGui::DragFloat("Scale", &state().status_scale, 0.01f, 0.1f, 2.f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(60);
				ImGui::DragInt("Opacity", &state().status_opacity, 1.f, 0, 255);
				ImGui::SetNextItemWidth(120);
				if (ImGui::TreeNode("Message Label")) {
					ImGui::Checkbox("Enable##msg", &state().message);
					ImGui::InputText("##message", &state().message_text);

				}
				if (ImGui::TreeNode("FPS Label")) {
					ImGui::Checkbox("Enable##fps", &state().fps_label);
					ImGui::Checkbox("Prefix##fps", &state().fps_prefix);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("CPS Label")) {
					ImGui::Checkbox("Enable##cps", &state().cps_label);
					ImGui::Checkbox("Prefix##cps", &state().cps_prefix);
					ImGui::Checkbox("Total clicks", &state().cps_total);
				}
				ImGui::Checkbox("Best run label", &state().best_run);
				ImGui::Checkbox("Attempts label", &state().attempts_label);
				ImGui::TreePop();
			}
			//if (ImGui::TreeNode("Extra Icons")) {
				//ImGui::SetNextItemWidth(60);
				//if(ImGui::DragInt("Selected Dart", &state().selected_dart, 1.f, 1, 255)) {
				//	Icons::patchDart(Icons::getCount("dart", "001"), state().selected_dart);
				//}
				/*ImGui::SetNextItemWidth(60);
				if (ImGui::DragInt("Ship Amount", &state().ship_amount, 1.f, 1, 1000)) {
					Icons::patchShip();
				}
				ImGui::SetNextItemWidth(60);
				if (ImGui::DragInt("Ball Amount", &state().ball_amount, 1.f, 1, 1000)) {
					Icons::patchBall();
				}
				ImGui::SetNextItemWidth(60);
				if (ImGui::DragInt("UFO Amount", &state().bird_amount, 1.f, 1, 1000)) {
					Icons::patchBird();
				}*/
				//ImGui::TreePop();
			//}
		}
		ImGui::End();

		if (state().explorer_enabled)
			render_explorer_window(state().explorer_enabled);

	}
	state().just_loaded = false;
	// this is so hacky lmao
	// so for some reason when you alt tab, the fps bypass resets to 60fps
	// i woudlve debugged it to see where it came from, but for some reason
	// 1.9 has antidebugging and i dont feel like figuring out how to bypass it
	if (state().fps_bypass && CCDirector::sharedDirector()->getAnimationInterval() != (1.0 / state().fps)) {
		update_fps_bypass();
	}

	if (state().color_picker) {
		static bool just_opened = true;
		static Color3F color;
		if (just_opened) {
			ImGui::OpenPopup("Color picker");
			just_opened = false;
			color = Color3F::from(state().color_picker->getColorValue());
		}
		bool unused_open = true;
		if (ImGui::BeginPopupModal("Color picker", &unused_open,
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {

			ImGui::ColorPicker3("##color_picker.color", &color.r);

			ImGui::EndPopup();
		} else {
			state().color_picker->setColorValue(color);
			state().color_picker = nullptr;
			just_opened = true;
		}
	}

	if (ImGui::GetTime() < 5.0 && !state().visible) {
		ImGui::SetNextWindowPos({ 10, 10 });
		ImGui::Begin("startupmsgwindow", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text("Mat's nice hacks loaded, press F1 or ~ to show menu");
		ImGui::End();
	}

	ImGui::PopFont();
}

void imgui_init() {
	ImGui::GetIO().Fonts->AddFontFromFileTTF("Muli-SemiBold.ttf", 18.f);

	auto& style = ImGui::GetStyle();
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowBorderSize = 0;

	auto colors = style.Colors;
	colors[ImGuiCol_FrameBg] = ImVec4(0.31f, 0.31f, 0.31f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.59f, 0.59f, 0.59f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.61f, 0.61f, 0.61f, 0.67f);
	colors[ImGuiCol_TitleBg] = colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.89f, 0.89f, 0.89f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.35f);
}

void setup_imgui_menu() {
	ImGuiHook::setToggleCallback([]() { state().visible = !state().visible; });
	ImGuiHook::setRenderFunction(imgui_render);
	ImGuiHook::setInitFunction(imgui_init);

	ImGuiHook::setupHooks([](auto addr, auto hook, auto orig) {
		MH_CreateHook(addr, hook, orig);
		MH_EnableHook(addr);
	});
}