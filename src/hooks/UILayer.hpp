#include <cocos2d.h>
#include "../gd.hpp"
#include "../utils.hpp"
#include "../state.hpp"
#include "../menu.hpp"
#include "../StartposSwitcher.hpp"
#include "../state.hpp"
#include "matdash.hpp"

class UILayer : public gd::UILayer {
public:
	bool _init();
	static void initHooks();
};