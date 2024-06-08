#include "PlayerObject.hpp"
#include <matdash.hpp>

using namespace matdash;

void PlayerObject::updatePlayerFrame(int frameID) {
	if (state().use_mini_icon) return orig<&PlayerObject::updatePlayerFrame>(this, 0);
	orig<&PlayerObject::updatePlayerFrame>(this, frameID);
}

void PlayerObject::updatePlayerRollFrame(int frameID) {
	if (state().use_mini_icon) return orig<&PlayerObject::updatePlayerRollFrame>(this, 0);
	orig<&PlayerObject::updatePlayerRollFrame>(this, frameID);
}

bool PlayerObject::init_(int frame, int type, CCLayer* layer) {
	auto init = orig<&PlayerObject::init_>(this, frame, type, layer);
	if (state().use_mini_icon) orig<&PlayerObject::updatePlayerFrame>(this, 0);
	return init;
}

void PlayerObject::destroyPlayer(bool _idk) {
	if (state().no_death_effect) {
		this->stopActionByTag(0);
		this->stopActionByTag(1);
		auto particle1 = MBO(CCParticleSystemQuad*, this, 0x40c);
		auto particle2 = MBO(CCParticleSystemQuad*, this, 0x410);
		auto particle3 = MBO(CCParticleSystemQuad*, this, 0x414);
		auto particle4 = MBO(CCParticleSystemQuad*, this, 0x418);
		if (MBO(bool, this, 0x3bd)) {
			particle1->stopSystem();
		}
		particle2->stopSystem();
		particle3->stopSystem();
		particle4->stopSystem();
		return;
	}
	orig<&PlayerObject::destroyPlayer>(this, _idk);
}

void PlayerObject::initHooks() {
	matdash::add_hook<&PlayerObject::init_>(gd::base + 0xd8ca0);
	matdash::add_hook<&PlayerObject::updatePlayerFrame>(gd::base + 0xdfff0);
	matdash::add_hook<&PlayerObject::updatePlayerRollFrame>(gd::base + 0xe0430);
	matdash::add_hook<&PlayerObject::destroyPlayer>(gd::base + 0xddda0);
}