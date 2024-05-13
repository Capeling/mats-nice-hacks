#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <cocos2d.h>
#include <matdash.hpp>
#include <matdash/minhook.hpp>
#include <matdash/boilerplate.hpp>
#include "console.hpp"
#include "gd.hpp"
#include <filesystem>
#include <algorithm>
#include "utils.hpp"
#include "state.hpp"
#include "menu.hpp"
#include "preview-mode.hpp"
#include "save-file.hpp"
#include "lvl-share.hpp"
#include <cocos-ext.h>
#include <imgui-hook.hpp>

#include <cmath>
#undef isnan

#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define MBO MEMBERBYOFFSET

using namespace matdash;

cc::stdcall<int> FMOD_setVolume(FMOD::Channel* sound, float v) {
	if (state().speed_hack_enabled && v >= 0.f)
		sound->setPitch(state().speed);
	return orig<&FMOD_setVolume>(sound, v);
}

void CCKeyboardDispatcher_dispatchKeyboardMSG(CCKeyboardDispatcher* self, int key, bool down) {
	if (down) {
		if (key == 'R' && state().has_retry_keybind) {
			auto pl = GameManager::sharedState()->getPlayLayer();
			if (pl) {
				pl->resetLevel();
				return;
			}
		}
	}
	orig<&CCKeyboardDispatcher_dispatchKeyboardMSG>(self, key, down);
}

CCPoint* EditorUI_getLimitedPosition(CCPoint* retVal, CCPoint point) {
	*retVal = point;
	return retVal;
}

void PlayLayer_spawnPlayer2(PlayLayer* self) {
	self->player2()->setVisible(true);
	orig<&PlayLayer_spawnPlayer2>(self);
}

static bool g_holding_in_editor = false;

void EditorUI_onPlaytest(EditorUI* self, void* btn) {
	if (!g_holding_in_editor)
		return orig<&EditorUI_onPlaytest>(self, btn);
}

void EditorUI_ccTouchBegan(EditorUI* self, void* idc, void* idc2) {
	g_holding_in_editor = true;
	return orig<&EditorUI_ccTouchBegan>(self, idc, idc2);
}

void EditorUI_ccTouchEnded(EditorUI* self, void* idc, void* idc2) {
	g_holding_in_editor = false;
	return orig<&EditorUI_ccTouchEnded>(self, idc, idc2);
}

bool ColorSelectPopup_init(ColorSelectPopup* self, GameObject* obj, int color_id, int idk, int idk2) {
	state().should_fix_hue = true;
	if (!orig<&ColorSelectPopup_init>(self, obj, color_id, idk, idk2)) return false;

	auto director = CCDirector::sharedDirector();
	auto winSize = director->getWinSize();

	constexpr auto handler = [](CCObject* _self, CCObject* button) {
		auto self = reinterpret_cast<ColorSelectPopup*>(_self);
		auto picker = self->colorPicker();
		state().color_picker = picker;
	};
	auto sprite = ButtonSprite::create("picker", 0x28, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 30.0);

	auto button = CCMenuItemSpriteExtra::create(sprite, nullptr, self, to_handler<SEL_MenuHandler, handler>);
	const auto win_size = CCDirector::sharedDirector()->getWinSize();
	const auto menu = self->menu();
	button->setPosition(menu->convertToNodeSpace({ win_size.width - 50.f, win_size.height - 110 }));
	menu->addChild(button);

	return true;
}

void ColorSelectPopup_dtor(ColorSelectPopup* self) {
	state().should_fix_hue = false;
	orig<&ColorSelectPopup_dtor>(self);
}

void EditorPauseLayer_customSetup(EditorPauseLayer* self) {
	orig<&EditorPauseLayer_customSetup>(self);

	constexpr auto handler = [](CCObject* self, CCObject*) {
		auto text = clipboard::read();
		auto editor = reinterpret_cast<LevelEditorLayer*>(reinterpret_cast<CCNode*>(self)->getParent());
		editor->editorUI()->pasteObjects(text);
	};

	auto sprite = ButtonSprite::create("paste string", 0x28, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 30.0);
	auto button = CCMenuItemSpriteExtra::create(sprite, nullptr, self, to_handler<SEL_MenuHandler, handler>);
	const auto win_size = CCDirector::sharedDirector()->getWinSize();
	auto menu = CCMenu::create();
	menu->setPosition({0, 0});
	button->setPosition({ win_size.width - 50.f, 300.f });
	menu->addChild(button);
	self->addChild(menu);
}

void AppDelegate_trySaveGame(AppDelegate* self) {
	orig<&AppDelegate_trySaveGame>(self);

	state().save();
}

bool EditorUI_init(EditorUI* self, LevelEditorLayer* editor) {
	if (!orig<&EditorUI_init>(self, editor)) return false;

	if (!state().saved_clipboard.empty()) {
		self->clipboard() = state().saved_clipboard;
		self->updateButtons();
	}

	return true;
}

void EditorUI_dtor(EditorUI* self) {
	state().saved_clipboard = self->clipboard();
	orig<&EditorUI_dtor>(self);
}

void LevelInfoLayer_onClone(LevelInfoLayer* self, CCObject* foo) {
	orig<&LevelInfoLayer_onClone>(self, foo);
	if (!self->shouldDownloadLevel()) {
		auto level = static_cast<GJGameLevel*>(LocalLevelManager::sharedState()->getLocalLevels()->objectAtIndex(0));
		level->songID() = self->level()->songID();
	}
}

void EditLevelLayer_onClone(EditLevelLayer* self) {
	orig<&EditLevelLayer_onClone>(self);
	// gd checks for this->field_0x130 == 0, but i have no idea what that is nor do i care
	auto level = static_cast<GJGameLevel*>(LocalLevelManager::sharedState()->getLocalLevels()->objectAtIndex(0));
	level->songID() = self->level()->songID();
}

bool CustomizeObjectLayer_init(CustomizeObjectLayer* self, GameObject* obj, CCArray* objs) {
	if (!orig<&CustomizeObjectLayer_init>(self, obj, objs)) return false;
	
	auto sprite = ButtonSprite::create("3DL", 0xdc, 0, 0.4, false, "bigFont.fnt", "GJ_button_04.png", 25.0);
	auto button = CCMenuItemSpriteExtra::create(sprite, nullptr, self, union_cast<SEL_MenuHandler>(base + 0x2e600));
	button->setTag(static_cast<int>(CustomColorMode::DL));
	button->setPosition(100, 0);
	from<CCArray*>(self, 0x1c4)->addObject(sprite);
	self->menu()->addChild(button);

	if (obj && obj->getColorMode() == CustomColorMode::DL) {
		self->hightlightSelected(sprite);
	}

	return true;
}

void MenuLayer_onMoreGames(MenuLayer* self, CCObject* sender) {
	const auto layer = FLAlertLayer::create(nullptr, "Success", "The level has been saved", "OK", nullptr, 320.f, false, 0);
	layer->show();
}

bool LevelSelectLayer_init(LevelSelectLayer* self, int page) {
	return orig<&LevelSelectLayer_init>(self, page);
}

static gd::string LevelTools_getAudioTitle(int audioID) {
	return std::string("hello");
}

GJGameLevel* LevelTools_getLevel(int levelID, bool __save) {
	return orig<&LevelTools_getLevel>(levelID, __save);
}

bool GJGarageLayer_init(GJGarageLayer* self) {
	return orig<&GJGarageLayer_init>(self);
}


matdash::cc::c_decl<cocos2d::extension::RGBA> cocos_hsv2rgb(cocos2d::extension::HSV color) {
	if ((state().should_fix_hue || state().always_fix_hue) && std::isnan(color.h))
		color.h = 0.0;
	return orig<&cocos_hsv2rgb>(color);
}

void PlayerObject_updatePlayerFrame(PlayerObject* self, int frameID) {
	if(state().use_mini_icon) return orig<&PlayerObject_updatePlayerFrame>(self, 0);
	orig<&PlayerObject_updatePlayerFrame>(self, frameID);
}

void PlayerObject_updatePlayerRollFrame(PlayerObject* self, int frameID) {
	if(state().use_mini_icon) return orig<&PlayerObject_updatePlayerRollFrame>(self, 0);
	orig<&PlayerObject_updatePlayerRollFrame>(self, frameID);
}

bool PlayerObject_init(PlayerObject* self, int frame, int type, int idk) {
	auto init = orig<&PlayerObject_init>(self, frame, type, idk);
	if(state().use_mini_icon) orig<&PlayerObject_updatePlayerFrame>(self, 0);
	return init;
}

int labelCount = 0;
float updateRgb = 0.0f;

CCMenu* labelMenu = nullptr;

CCLabelBMFont* fpsLabel = nullptr;
CCLabelBMFont* attemptsLabel = nullptr;
CCLabelBMFont* cpsLabel = nullptr;

std::vector<time_t> s_clickFrames;
int totalClicks = 0;

CCLabelBMFont* createStandardLabel() {
	auto label = CCLabelBMFont::create("", "bigFont.fnt");
	label->setAnchorPoint({0.f, 0.5f});
	label->setScale(0.5f);
	label->setZOrder(999999);
	label->setOpacity(255 / 2);
	labelCount++;
    return label;
}

void createFpsLabel(PlayLayer* self) {
	fpsLabel = createStandardLabel();
	labelMenu->addChild(fpsLabel);
}

void createAttemptsLabel(PlayLayer* self) {
	attemptsLabel = createStandardLabel();
	labelMenu->addChild(attemptsLabel);
}

void createCpsLabel(PlayLayer* self) {
	cpsLabel = createStandardLabel();
	labelMenu->addChild(cpsLabel);
}

void updateAttemptLabel(PlayLayer* self) {
    if(attemptsLabel) {
        int attemptCount = self->attemptsCount() + 1;
        attemptsLabel->setString(CCString::createWithFormat("Attempt %i", attemptCount)->getCString());
    }
}

void updateLabels(PlayLayer* self) {

	auto director = CCDirector::sharedDirector();

	if(labelMenu) {
		if(state().fps_label && !fpsLabel) {
			createFpsLabel(self);
		}

		if(state().attempts_label && !attemptsLabel) {
			createAttemptsLabel(self);
            updateAttemptLabel(self);
		}

		if(state().cps_label && !cpsLabel) {
			createCpsLabel(self);
		}

		if(fpsLabel) {
			fpsLabel->setString(std::to_string(static_cast<int>(ImGui::GetIO().Framerate)).c_str());
			if(!state().fps_label) {
				fpsLabel->removeFromParent();
				fpsLabel = nullptr;
				labelCount--;
			}
		}


        if(cpsLabel) {
			std::string cpsCurrent = CCString::createWithFormat("%i", s_clickFrames.size())->getCString();
			std::string cpsTotal = CCString::createWithFormat("/%i", totalClicks)->getCString();
			std::string prefix = "CPS: ";

			if(!state().cps_prefix) prefix.clear();
			if(!state().cps_total) cpsTotal.clear();

            cpsLabel->setString(std::string(prefix + cpsCurrent + cpsTotal).c_str());
			if(!state().cps_label) {
				cpsLabel->removeFromParent();
				cpsLabel = nullptr;
				labelCount--;
			}
        }

        if(!state().attempts_label && attemptsLabel) {
            attemptsLabel->removeFromParent();
			attemptsLabel = nullptr;
            labelCount--;
        }
		labelMenu->setPosition({director->getScreenLeft() + 5.f, director->getScreenTop() - (labelCount * 10) + labelCount});
		labelMenu->alignItemsVerticallyWithPadding(1.f);
	}
}

void PlayLayer_resetLevel(PlayLayer* self) {
    s_clickFrames.clear();
	totalClicks = 0;
    orig<&PlayLayer_resetLevel>(self);
}

bool hasClicked = false;

void PlayLayer_pushButton(PlayLayer* self, int idk1, bool idk2) {
    if (!hasClicked) {
        s_clickFrames.push_back(time::getTime());
		totalClicks++;
        hasClicked = true;
    }
    orig<&PlayLayer_pushButton>(self, idk1, idk2);
}

bool PlayLayer_init(PlayLayer* self, GJGameLevel* level) {
	if (!orig<&PlayLayer_init>(self, level)) return false;
	labelCount = 0;
	totalClicks = 0;
	s_clickFrames.clear();

	auto director = CCDirector::sharedDirector();
	auto winSize = director->getWinSize();

	labelMenu = CCMenu::create();
	labelMenu->setZOrder(999999);
	self->addChild(labelMenu);

	if(state().fps_label) {
		createFpsLabel(self);
	}
	if(state().cps_label) {
		createCpsLabel(self);
	}
	if(state().attempts_label) {
		createAttemptsLabel(self);
		updateAttemptLabel(self);
	}


	if (state().show_percent) {
		auto gm = GameManager::sharedState();
		const auto bar = gm->getShowProgressBar();

		auto label = CCLabelBMFont::create("0.00%", "bigFont.fnt");
		label->setAnchorPoint({ bar ? 0.f : 0.5f, 0.5f });
		label->setScale(0.5f);
		label->setZOrder(999999);
		label->setPosition({ winSize.width / 2.f + (bar ? 107.2f : 0.f), winSize.height - 8.f });
		label->setTag(12345);
		self->addChild(label);
	}
	if(state().hide_attempts) {
		self->attemptsLabel()->setVisible(false);
	}

	updateLabels(self);

	return true;
}

ccColor3B getChromaColour() {
	return ColorUtility::hsvToRgb(cchsv((updateRgb * 180) / 10.0f, 1.0f, 1.0f, true, true));
}


void PlayLayer_updateAttempts(PlayLayer* self) {
	updateAttemptLabel(self);
	orig<&PlayLayer_updateAttempts>(self);
}

cc::thiscall<void> PlayLayer_update(PlayLayer* self, float dt) {
	orig<&PlayLayer_update>(self, dt);
	auto label = self->getChildByTag(12345);
	if (label) {
		const auto value = std::min(self->player1()->getPositionX() / self->levelLength(), 1.f) * 100.f;
		reinterpret_cast<CCLabelBMFont*>(label)->setString(CCString::createWithFormat("%.2f%%", value)->getCString());
	}

	updateLabels(self);

	if (state().rainbow_color) { //nobody has to know

		auto player1 = self->player1();
		auto player2 = self->player2();

		auto playerGlowNode = self->playerGlowNode();

		updateRgb += dt * state().rainbow_speed;

		for(size_t i = 0; i < player1->getChildrenCount(); i++) {
			auto node = static_cast<CCSprite*>(player1->getChildren()->objectAtIndex(i));
			node->setColor(getChromaColour());
		}

		for(size_t i = 0; i < player2->getChildrenCount(); i++) {
			auto node = static_cast<CCSprite*>(player2->getChildren()->objectAtIndex(i));
			for(size_t i = 0; i < node->getChildrenCount(); i++) {
				auto node2 = static_cast<CCSprite*>(node->getChildren()->objectAtIndex(i));
				node2->setColor(getChromaColour());
			}
		}
		
		if(auto player2Glow = static_cast<CCSprite*>(playerGlowNode->getChildren()->objectAtIndex(playerGlowNode->getChildrenCount()))) {
			for(size_t i = 0; i < player2Glow->getChildrenCount(); i++) {
				auto node = static_cast<CCSprite*>(player2Glow->getChildren()->objectAtIndex(i));
				node->setColor(getChromaColour());
			}
		};
	}

	// sick
	if (state().hide_player) {
		self->player1()->setVisible(false);
		self->player2()->setVisible(false);
	}

	time_t currentTick = time::getTime();
	s_clickFrames.erase(std::remove_if(s_clickFrames.begin(), s_clickFrames.end(), [currentTick](float tick) {
		return currentTick - tick > 1000;
	}), s_clickFrames.end());

	hasClicked = false;
	return {};
}

void mod_main(HMODULE) {
	//static Console console;
	std::cout << std::boolalpha;

	state().load();

	// static_assert(sizeof(CCObject) == 24, "CCObject is wrong!");
	// static_assert(sizeof(CCNode) == 0xe8, "CCNode size is wrong");
	// static_assert(sizeof(CCNodeRGBA) == 0xf8, "CCNodeRGBA wrong");
	// static_assert(sizeof(CCSprite) == 0x1b8, "CCSprite wrong");
	// static_assert(sizeof(CCDirector) == 0x100, "CCDirector size is wrong!");

	// static_assert(offsetof(CCDirector, m_pRunningScene) == 0xa8, "this is wrong!");
	// static_assert(offsetof(CCDirector, m_pobOpenGLView) == 0x68, "This is wrong!");

	auto cocos = GetModuleHandleA("libcocos2d.dll");
	auto cocos_ext = GetModuleHandleA("libExtensions.dll");
	add_hook<&FMOD_setVolume>(GetProcAddress(FMOD::base, "?setVolume@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@M@Z"));
	add_hook<&CCKeyboardDispatcher_dispatchKeyboardMSG>(GetProcAddress(cocos, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z"));
	// add_hook<&MenuLayer_init>(base + 0xaf210);
	add_hook<&EditorUI_getLimitedPosition, matdash::Stdcall>(base + 0x4b500);

	// for invisible dual fix (thx pie)
	add_hook<&PlayLayer_spawnPlayer2>(base + 0xef0d0);

	// for freeze fix (thx cos8o)
	add_hook<&EditorUI_onPlaytest>(base + 0x489c0);
	add_hook<&EditorUI_ccTouchBegan>(base + 0x4d5e0);
	add_hook<&EditorUI_ccTouchEnded>(base + 0x4de40);

	add_hook<&AppDelegate_trySaveGame>(base + 0x293f0);

	add_hook<&PlayLayer_init>(base + 0xe35d0);
	add_hook<&PlayLayer_update>(base + 0xe9360);
	add_hook<&PlayLayer_updateAttempts>(base + 0xf33a0);
	add_hook<&PlayLayer_resetLevel>(base + 0xf1f20);
	add_hook<&PlayLayer_pushButton>(base + 0xf0a00);

	add_hook<&ColorSelectPopup_init>(base + 0x29db0);
	add_hook<&ColorSelectPopup_dtor>(base + 0x2b050);

	add_hook<&EditorPauseLayer_customSetup>(base + 0x3e3d0);

	add_hook<&EditorUI_init>(base + 0x3fdc0);
	add_hook<&EditorUI_dtor>(base + 0x3fb90);

	add_hook<&LevelInfoLayer_onClone>(base + 0x9e2c0);
	add_hook<&EditLevelLayer_onClone>(base + 0x3da30);

	add_hook<&CustomizeObjectLayer_init>(base + 0x2dc70);

	add_hook<&PlayerObject_init>(base + 0xd8ca0);
	add_hook<&PlayerObject_updatePlayerFrame>(base + 0xdfff0);
	add_hook<&PlayerObject_updatePlayerRollFrame>(base + 0xe0430);

	add_hook<&GJGarageLayer_init>(base + 0x7c5c0);

	//add_hook<&LevelTools_getAudioTitle>(base + 0xa9ad0);
	//add_hook<&LevelTools_getLevel>(base + 0xa9280);

	add_hook<&cocos_hsv2rgb>(GetProcAddress(cocos_ext, "?RGBfromHSV@CCControlUtils@extension@cocos2d@@SA?AURGBA@23@UHSV@23@@Z"));

	// add_hook<&MenuLayer_onMoreGames>(base + 0xb0070);

	preview_mode::init();
	// save_file::init();
	lvl_share::init();

	setup_imgui_menu();

	// patches for the editor extension

	patch(base + 0x4ca75, { 0xeb });
	patch(base + 0x4ca45, { 0xeb });
	patch(base + 0x4779c, { 0xeb });
	patch(base + 0x477b9, { 0xeb });

	patch(base + 0x4b445, { 0xeb, 0x44 });
}
