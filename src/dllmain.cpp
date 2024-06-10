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
#include "hooks/PlayerObject.hpp"
#include "hooks/PlayLayer.hpp"
#include "hooks/EditorUI.hpp"
#include "hooks/GJGarageLayer.hpp"
#include "Icons.hpp"

#include <cmath>
#undef isnan

using namespace matdash;

cc::stdcall<int> FMOD_setVolume(gd::FMOD::Channel* sound, float v) {
	if (state().speed_hack_enabled && v >= 0.f)
		sound->setPitch(state().speed);
	return orig<&FMOD_setVolume>(sound, v);
}

void CCKeyboardDispatcher_dispatchKeyboardMSG(CCKeyboardDispatcher* self, int key, bool down) {
	if (down) {
		if (key == 'R' && state().has_retry_keybind) {
			auto pl = gd::GameManager::sharedState()->getPlayLayer();
			if (pl) {
				pl->resetLevel();
				return;
			}
		}
	}
	orig<&CCKeyboardDispatcher_dispatchKeyboardMSG>(self, key, down);
}



bool ColorSelectPopup_init(gd::ColorSelectPopup* self, gd::GameObject* obj, int color_id, int idk, int idk2) {
	state().should_fix_hue = true;
	if (!orig<&ColorSelectPopup_init>(self, obj, color_id, idk, idk2)) return false;

	auto director = CCDirector::sharedDirector();
	auto winSize = director->getWinSize();

	constexpr auto handler = [](CCObject* _self, CCObject* button) {
		auto self = reinterpret_cast<gd::ColorSelectPopup*>(_self);
		auto picker = self->colorPicker();
		state().color_picker = picker;
	};
	auto sprite = gd::ButtonSprite::create("picker", 0x28, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 30.0);

	auto button = gd::CCMenuItemSpriteExtra::create(sprite, nullptr, self, to_handler<SEL_MenuHandler, handler>);
	const auto win_size = CCDirector::sharedDirector()->getWinSize();
	const auto menu = self->menu();
	button->setPosition(menu->convertToNodeSpace({ win_size.width - 50.f, win_size.height - 110 }));
	menu->addChild(button);

	return true;
}

void ColorSelectPopup_dtor(gd::ColorSelectPopup* self) {
	state().should_fix_hue = false;
	orig<&ColorSelectPopup_dtor>(self);
}

void EditorPauseLayer_customSetup(gd::EditorPauseLayer* self) {
	orig<&EditorPauseLayer_customSetup>(self);

	constexpr auto handler = [](CCObject* self, CCObject*) {
		auto text = clipboard::read();
		auto editor = reinterpret_cast<gd::LevelEditorLayer*>(reinterpret_cast<CCNode*>(self)->getParent());
		editor->editorUI()->pasteObjects(text);
	};

	auto sprite = gd::ButtonSprite::create("paste string", 0x28, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 30.0);
	auto button = gd::CCMenuItemSpriteExtra::create(sprite, nullptr, self, to_handler<SEL_MenuHandler, handler>);
	const auto win_size = CCDirector::sharedDirector()->getWinSize();
	auto menu = CCMenu::create();
	menu->setPosition({0, 0});
	button->setPosition({ win_size.width - 50.f, 300.f });
	menu->addChild(button);
	self->addChild(menu);
}

void AppDelegate_trySaveGame(gd::AppDelegate* self) {
	orig<&AppDelegate_trySaveGame>(self);

	state().save();
}


void LevelInfoLayer_onClone(gd::LevelInfoLayer* self, CCObject* foo) {
	orig<&LevelInfoLayer_onClone>(self, foo);
	if (!self->shouldDownloadLevel()) {
		auto level = static_cast<gd::GJGameLevel*>(gd::LocalLevelManager::sharedState()->getLocalLevels()->objectAtIndex(0));
		level->songID() = self->level()->songID();
	}
}

void EditLevelLayer_onClone(gd::EditLevelLayer* self) {
	orig<&EditLevelLayer_onClone>(self);
	// gd checks for this->field_0x130 == 0, but i have no idea what that is nor do i care
	auto level = static_cast<gd::GJGameLevel*>(gd::LocalLevelManager::sharedState()->getLocalLevels()->objectAtIndex(0));
	level->songID() = self->level()->songID();
}

bool CustomizeObjectLayer_init(gd::CustomizeObjectLayer* self, gd::GameObject* obj, CCArray* objs) {
	if (!orig<&CustomizeObjectLayer_init>(self, obj, objs)) return false;
	
	auto sprite = gd::ButtonSprite::create("3DL", 0xdc, 0, 0.4, false, "bigFont.fnt", "GJ_button_04.png", 25.0);
	auto button = gd::CCMenuItemSpriteExtra::create(sprite, nullptr, self, union_cast<SEL_MenuHandler>(gd::base + 0x2e600));
	button->setTag(static_cast<int>(gd::CustomColorMode::DL));
	button->setPosition(100, 0);
	from<CCArray*>(self, 0x1c4)->addObject(sprite);
	self->menu()->addChild(button);

	if (obj && obj->getColorMode() == gd::CustomColorMode::DL) {
		self->hightlightSelected(sprite);
	}

	return true;
}

void MenuLayer_onMoreGames(gd::MenuLayer* self, CCObject* sender) {
	const auto layer = gd::FLAlertLayer::create(nullptr, "Test", "", "OK", nullptr, 320.f, false, 0);
	layer->show();
}

bool LevelSelectLayer_init(gd::LevelSelectLayer* self, int page) {
	return orig<&LevelSelectLayer_init>(self, page);
}

static gd::string LevelTools_getAudioTitle(int audioID) {
	return std::string("hello");
}

gd::GJGameLevel* LevelTools_getLevel(int levelID, bool __save) {
	return orig<&LevelTools_getLevel>(levelID, __save);
}

bool GJGarageLayer_init(gd::GJGarageLayer* self) {
	return orig<&GJGarageLayer_init>(self);
}

matdash::cc::c_decl<cocos2d::extension::RGBA> cocos_hsv2rgb(cocos2d::extension::HSV color) {
	if ((state().should_fix_hue || state().always_fix_hue) && std::isnan(color.h))
		color.h = 0.0;
	return orig<&cocos_hsv2rgb>(color);
}

int getCount(std::string startLabel, std::string end) { // taken from https://github.com/Alphalaneous/ExtraIcons/blob/main/src/main.cpp

	int count = 1;
	while (true) {

		std::stringstream number;
		number << std::setw(2) << std::setfill('0') << count;

		if (CCSprite::createWithSpriteFrameName((startLabel + "_" + number.str() + "_" + end + ".png").c_str())) {
			count++;
		}
		else {
			break;
		}
	}
	std::cout << startLabel << ": " << count - 1 << "\n";
	return count - 1;
}

bool MenuLayer_init(gd::MenuLayer* self) {
	if (!orig<&MenuLayer_init>(self))
		return false;

	Icons::patchCube(getCount("player", "001"));
	Icons::patchShip(getCount("ship", "001"));
	Icons::patchBall(getCount("player_ball", "001"));
	Icons::patchBird(getCount("bird", "001"));

	return true;
}

bool GameManager_isIconUnlocked(gd::GameManager* self, int id, int type) {
	switch (type) {
	case 0: if (id > 48) return true; break;
	case 1: if (id > 18) return true; break;
	case 2: if (id > 10) return true; break;
	case 3: if (id > 10) return true; break;
	}
	return orig<&GameManager_isIconUnlocked>(self, id, type);
}

void getCreateBtn() {
	return reinterpret_cast<void(__fastcall*)()>(gd::base + 0x47200)();
}

void objectAdd(int id)
{
	__asm {
		push ecx
		push 0x4
		push id
		mov ecx, ebx
		call getCreateBtn
		push eax
		mov ecx, esi
		call edi
	}
}

void ObjectToolboxAdd_OrbTab() { //adds secret coin to the editor xd
	objectAdd(0x8e);
	orig<&ObjectToolboxAdd_OrbTab>();
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
	add_hook<&FMOD_setVolume>(GetProcAddress(gd::FMOD::base, "?setVolume@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@M@Z"));
	add_hook<&CCKeyboardDispatcher_dispatchKeyboardMSG>(GetProcAddress(cocos, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z"));
	add_hook<&MenuLayer_init>(gd::base + 0xaf210);

	// for freeze fix (thx cos8o)

	add_hook<&AppDelegate_trySaveGame>(gd::base + 0x293f0);

	add_hook<&ColorSelectPopup_init>(gd::base + 0x29db0);
	add_hook<&ColorSelectPopup_dtor>(gd::base + 0x2b050);

	add_hook<&EditorPauseLayer_customSetup>(gd::base + 0x3e3d0);


	add_hook<&LevelInfoLayer_onClone>(gd::base + 0x9e2c0);
	add_hook<&EditLevelLayer_onClone>(gd::base + 0x3da30);

	add_hook<&CustomizeObjectLayer_init>(gd::base + 0x2dc70);

	add_hook<&GameManager_isIconUnlocked>(gd::base + 0x66b10);

	//add_hook<&ObjectToolboxAdd_OrbTab>(gd::base + 0x45d08);

	//add_hook<&LevelTools_getAudioTitle>(base + 0xa9ad0);
	//add_hook<&LevelTools_getLevel>(base + 0xa9280);

	add_hook<&cocos_hsv2rgb>(GetProcAddress(cocos_ext, "?RGBfromHSV@CCControlUtils@extension@cocos2d@@SA?AURGBA@23@UHSV@23@@Z"));

	add_hook<&MenuLayer_onMoreGames>(gd::base + 0xb0070);

	preview_mode::init();
	PlayLayer::initHooks();
	GJGarageLayer::initHooks();
	PlayerObject::initHooks();
	EditorUI::initHooks();
	// save_file::init();
	lvl_share::init();

	setup_imgui_menu();

	// patches for the editor extension

	patch(gd::base + 0x4ca75, { 0xeb });
	patch(gd::base + 0x4ca45, { 0xeb });
	patch(gd::base + 0x4779c, { 0xeb });
	patch(gd::base + 0x477b9, { 0xeb });

	patch(gd::base + 0x4b445, { 0xeb, 0x44 });

	//patch(base + 0xf05d0, { 0x90, 0x90, 0x90, 0x90, 0x90 });
	//patch(base + 0xf05f4, { 0x90, 0x90, 0x90, 0x90, 0x90 });

	//patch(base + 0xdde9e, { 0x83, 0xc4, 0x04, 0x90, 0x90, 0x90 }); no death effect, wip the particle and circle effect still there
}
