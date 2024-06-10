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
	static void initHooks();
};