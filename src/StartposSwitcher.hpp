#include <cocos2d.h>
#include "gd.hpp"
#include "utils.hpp"
#include "state.hpp"
#include "menu.hpp"
#include "state.hpp"
#include "matdash.hpp"

using namespace matdash;

class StartposSwitcher {
public:
	CCMenu* m_switcherMenu = nullptr;
	CCLabelBMFont* m_switcherLabel = nullptr;
	gd::CCMenuItemSpriteExtra* m_prevBtn = nullptr;
	gd::CCMenuItemSpriteExtra* m_nextBtn = nullptr;
};