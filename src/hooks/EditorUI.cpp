#include "EditorUI.hpp"

using namespace matdash;

static bool holdingInEditor = false;

CCPoint EditorUI::getLimitedPosition(CCPoint point) {
	return orig<&EditorUI::getLimitedPosition>(this, point);
}

void EditorUI::onPlaytest(void* btn) {
		if (!holdingInEditor)
	return orig<&EditorUI::onPlaytest>(this, btn);
}

bool EditorUI::ccTouchBegan_(cocos2d::CCTouch* ccTouch, cocos2d::CCEvent* ccEvent) {
	holdingInEditor = true;
	return orig<&EditorUI::ccTouchBegan>(this, ccTouch, ccEvent);
}

void EditorUI::ccTouchEnded_(cocos2d::CCTouch* ccTouch, cocos2d::CCEvent* ccEvent) {
	holdingInEditor = false;
	return orig<&EditorUI::ccTouchEnded>(this, ccTouch, ccEvent);
}

bool EditorUI::init_(gd::LevelEditorLayer* editor) {
	if (!orig<&EditorUI::init_>(this, editor)) return false;

	if (!state().saved_clipboard.empty()) {
		this->clipboard() = state().saved_clipboard;
		this->updateButtons();
	}

	return true;
}

void EditorUI::dtor() {
	state().saved_clipboard = this->clipboard();
	orig<&EditorUI::dtor>(this);
}

void EditorUI::initHooks() {
	matdash::add_hook<&EditorUI::getLimitedPosition>(gd::base + 0x4b500);
	matdash::add_hook<&EditorUI::onPlaytest>(gd::base + 0x489c0);
	//matdash::add_hook<&EditorUI::ccTouchBegan_>(gd::base + 0x4d5e0);
	//matdash::add_hook<&EditorUI::ccTouchEnded_>(gd::base + 0x4de40);
	matdash::add_hook<&EditorUI::init_>(gd::base + 0x3fdc0);
	matdash::add_hook<&EditorUI::dtor>(gd::base + 0x3fb90);
}