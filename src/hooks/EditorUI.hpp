#include <cocos2d.h>
#include "../gd.hpp"
#include "../utils.hpp"
#include "../state.hpp"
#include "../menu.hpp"
#include "../state.hpp"
#include "matdash.hpp"

class EditorUI : public gd::EditorUI {
public:
	CCPoint getLimitedPosition(CCPoint point);
	void onPlaytest(void* btn);
	bool ccTouchBegan_(cocos2d::CCTouch* ccTouch, cocos2d::CCEvent* ccEvent);
	void ccTouchEnded_(cocos2d::CCTouch* ccTouch, cocos2d::CCEvent* ccEvent);
	bool init_(gd::LevelEditorLayer* editor);
	void dtor();
	static void initHooks();
};