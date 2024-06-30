#include <cocos2d.h>
#include "../gd.hpp"
#include "../utils.hpp"
#include "../state.hpp"
#include "../menu.hpp"
#include "../state.hpp"
#include "matdash.hpp"

class SimplePlayer : public gd::SimplePlayer {
public:
	bool init_(int frameID);
	void newExtraFrame(char const* extra);
	void updatePlayerFrame(int frameID, gd::IconType iconType);
	static void initHooks();
};