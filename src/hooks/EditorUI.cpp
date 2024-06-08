#include "EditorUI.hpp"

using namespace matdash;

static bool holdingInEditor = false;

CCPoint* EditorUI::getLimitedPosition(CCPoint* retVal, CCPoint point) {
	*retVal = point;
	return retVal;
}

void EditorUI::onPlaytest(void* btn) {
		if (!holdingInEditor)
			return orig<&EditorUI::onPlaytest>(this, btn);
}

void EditorUI::ccTouchBegan(void* idc, void* idc2) {
	holdingInEditor = true;
	return orig<&EditorUI::ccTouchBegan>(this, idc, idc2);
}

void EditorUI::ccTouchEnded(void* idc, void* idc2) {
	holdingInEditor = false;
	return orig<&EditorUI::ccTouchEnded>(this, idc, idc2);
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
	matdash::add_hook<&EditorUI::getLimitedPosition, matdash::Stdcall>(gd::base + 0x4b500);
	matdash::add_hook<&EditorUI::onPlaytest>(gd::base + 0x489c0);
	matdash::add_hook<&EditorUI::ccTouchBegan>(gd::base + 0x4d5e0);
	matdash::add_hook<&EditorUI::ccTouchEnded>(gd::base + 0x4de40);
	matdash::add_hook<&EditorUI::init_>(gd::base + 0x3fdc0);
	matdash::add_hook<&EditorUI::dtor>(gd::base + 0x3fb90);
}