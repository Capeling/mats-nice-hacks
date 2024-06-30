#include "SimplePlayer.hpp"
#include "../Icons.hpp"

using namespace matdash;

bool SimplePlayer::init_(int frameID) {
	if (!orig<&SimplePlayer::init_>(this, frameID))
		return false;

	int limit = Icons::getCount("player", "001");
	if (frameID > limit) frameID = limit;

	CCSprite* extraSprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("player_%02d_glow_001.png", frameID)->getCString());
	extraSprite->setTag(69);
	extraSprite->setVisible(false);
	extraSprite->setZOrder(99);
	extraSprite->setPosition(colorSpr()->getPosition());
	baseSpr()->addChild(extraSprite);

	newExtraFrame(CCString::createWithFormat("player_%02d_extra_001.png", frameID)->getCString());

	return true;
}

void SimplePlayer::newExtraFrame(char const* extra) {
	std::cout << extra << std::endl;
	auto extraSpr = static_cast<CCSprite*>(baseSpr()->getChildByTag(69));
	auto SFC = CCSpriteFrameCache::sharedSpriteFrameCache();
	if (!extraSpr) return;


	if (SFC->spriteFrameByName(extra)) {
		extraSpr->setPosition(colorSpr()->getPosition());
		extraSpr->setDisplayFrame(SFC->spriteFrameByName(extra));
		extraSpr->setVisible(true);
		std::cout << "isExtra" << std::endl;
	}
	else {
		extraSpr->setVisible(false);
		std::cout << "noExtra" << std::endl;
	}
}

void SimplePlayer::updatePlayerFrame(int frameID, gd::IconType iconType) {
	orig<&SimplePlayer::updatePlayerFrame>(this, frameID, iconType);
	const char* prefix = "player";
	switch (iconType) {
	case gd::IconType::Ship: prefix = "ship"; break;
	case gd::IconType::Ball: prefix = "player_ball"; break;
	case gd::IconType::UFO: prefix = "bird"; break;
	case gd::IconType::Wave: prefix = "dart"; break;
	default: break;
	}
	int limit = Icons::getCount(prefix, "001");
	std::cout << prefix << " " << limit << std::endl;
	if (frameID > limit) frameID = limit;
	newExtraFrame(CCString::createWithFormat("player_%02d_extra_001.png", frameID)->getCString());
}

void SimplePlayer::initHooks() {
	add_hook<&SimplePlayer::init_>(gd::base + 0x80810);
	add_hook<&SimplePlayer::updatePlayerFrame>(gd::base + 0x80c70);
}