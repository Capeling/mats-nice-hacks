#include <cocos2d.h>
#include "../gd.hpp"
#include "../utils.hpp"
#include "../state.hpp"
#include "../menu.hpp"
#include "../state.hpp"
#include "matdash.hpp"

class EditorUI : public gd::EditorUI {
public:
	CCPoint* getLimitedPosition(CCPoint* retVal, CCPoint point);
	void onPlaytest(void* btn);
	void ccTouchBegan(void* idc, void* idc2);
	void ccTouchEnded(void* idc, void* idc2);
	bool init_(gd::LevelEditorLayer* editor);
	void dtor();
	static void initHooks();
};