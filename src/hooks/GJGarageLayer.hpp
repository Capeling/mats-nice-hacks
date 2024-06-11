#include <cocos2d.h>
#include "../gd.hpp"
#include "../utils.hpp"
#include "../state.hpp"
#include "../menu.hpp"
#include "../state.hpp"
#include "matdash.hpp"

class GJGarageLayer : public gd::GJGarageLayer {
public:
	bool init_();
	void setupIconSelect();
	void selectPage(int iconType);
	void onDartIcon(CCObject*);
	void onNext(CCObject*);
	void onPrev(CCObject*);
	void switchToPage(int page);
	static void initHooks();
};

struct Fields {
	gd::CCMenuItemToggler* m_dartBtn = nullptr;
	CCSprite* m_dartSelector = nullptr;
	CCMenu* m_dartPage = nullptr;
	CCMenu* m_arrowMenu = nullptr;

	int m_wavePage = 0;
	int m_totalWavePages = 0;
};