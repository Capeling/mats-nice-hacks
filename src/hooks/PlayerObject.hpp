#include <cocos2d.h>
#include "../gd.hpp"
#include "../utils.hpp"
#include "../state.hpp"
#include "../menu.hpp"
#include "../state.hpp"

class PlayerObject : public gd::PlayerObject {
public:
	void updatePlayerFrame(int frameID);
	void updatePlayerRollFrame(int frameID);
	bool init_(int frame, int type, CCLayer* layer);
	void destroyPlayer(bool _idk);
	static void initHooks();
};