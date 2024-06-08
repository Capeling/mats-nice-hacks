#include <cocos2d.h>
#include "../gd.hpp"
#include "../utils.hpp"
#include "../state.hpp"
#include "../menu.hpp"
#include "../state.hpp"
#include "matdash.hpp"

class PlayLayer : public gd::PlayLayer {
public:
	bool init_(gd::GJGameLevel* level);
	void spawnPlayer2();
	void resetLevel();
	void pushButton(int idk1, bool idk2);
	void releaseButton(int idk1, bool idk2);
	void updateAttempts();
	void destroyPlayer(gd::PlayerObject* PlayerObject);
	matdash::cc::thiscall<void> update_(float dt);
	static void initHooks();
};